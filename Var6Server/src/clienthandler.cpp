#include "clienthandler.h"
#include "database.h"
#include "sha1.h"
#include "rsa.h"
#include "newtonsolver.h"
#include "audiosteganography.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QFile>
#include <cmath>

ClientHandler::ClientHandler(QTcpSocket *socket, QObject *parent)
    : QObject(parent), m_socket(socket)
{
    connect(m_socket, &QTcpSocket::readyRead,   this, &ClientHandler::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected,this, &ClientHandler::onDisconnected);
}

// ─────────────────────────── сеть ────────────────────────────

void ClientHandler::onReadyRead() {
    m_buffer += m_socket->readAll();
    // Обрабатываем все полные строки (разделитель — '\n')
    while (m_buffer.contains('\n')) {
        int idx  = m_buffer.indexOf('\n');
        QByteArray line = m_buffer.left(idx).trimmed();
        m_buffer = m_buffer.mid(idx + 1);
        if (!line.isEmpty())
            parseRequest(line);
    }
}

void ClientHandler::onDisconnected() {
    emit disconnected(this);
    m_socket->deleteLater();
}

void ClientHandler::sendResponse(const QJsonObject &obj) {
    QByteArray data = QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n";
    m_socket->write(data);
    m_socket->flush();
}

// ─────────────────────────── роутер ──────────────────────────

void ClientHandler::parseRequest(const QByteArray &data) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        sendResponse(errorResp("Некорректный JSON"));
        return;
    }

    QJsonObject req    = doc.object();
    QString     action = req["action"].toString();

    QJsonObject resp;
    if      (action == "register")      resp = handleRegister(req);
    else if (action == "login")         resp = handleLogin(req);
    else if (action == "rsa_encrypt")   resp = handleRsaEncrypt(req);
    else if (action == "rsa_decrypt")   resp = handleRsaDecrypt(req);
    else if (action == "sha1")          resp = handleSha1(req);
    else if (action == "newton")        resp = handleNewton(req);
    else if (action == "embed_audio")   resp = handleEmbedAudio(req);
    else if (action == "extract_audio") resp = handleExtractAudio(req);
    else if (action == "get_users")     resp = handleGetUsers(req);
    else if (action == "delete_user")   resp = handleDeleteUser(req);
    else if (action == "set_role")      resp = handleSetRole(req);
    else                                resp = errorResp("Неизвестное действие: " + action);

    sendResponse(resp);
}

// ────────────────────── auth ──────────────────────────────────

QJsonObject ClientHandler::handleRegister(const QJsonObject &req) {
    QString login = req["login"].toString().trimmed();
    QString pass  = req["password"].toString();

    if (login.isEmpty() || pass.isEmpty())
        return errorResp("Логин и пароль не могут быть пустыми");

    Database *db = Database::getInstance();
    if (db->getUser(login).isValid())
        return errorResp("Пользователь уже существует");

    User u;
    u.login        = login;
    u.passwordHash = SHA1::hash(pass);
    u.role         = "user";

    if (!db->saveUser(u))
        return errorResp("Ошибка сохранения пользователя");

    return okResp("Регистрация успешна");
}

QJsonObject ClientHandler::handleLogin(const QJsonObject &req) {
    QString login = req["login"].toString().trimmed();
    QString pass  = req["password"].toString();

    Database *db = Database::getInstance();
    User u = db->getUser(login);

    if (!u.isValid() || !SHA1::verify(pass, u.passwordHash))
        return errorResp("Неверный логин или пароль");

    m_currentUser = u;

    QJsonObject resp = okResp("Вход выполнен");
    resp["user"] = u.toJson();
    return resp;
}

// ──────────────────────── RSA ─────────────────────────────────

QJsonObject ClientHandler::handleRsaEncrypt(const QJsonObject &req) {
    QString text = req["text"].toString();
    if (text.isEmpty()) return errorResp("Пустой текст");

    RSA::KeyPair kp = RSA::generateKeys();
    QByteArray enc  = RSA::encrypt(text.toUtf8(), kp.n, kp.e);

    QJsonObject resp = okResp();
    resp["encrypted"] = QString::fromLatin1(enc.toBase64());
    resp["keys"]      = RSA::keyPairToString(kp);
    return resp;
}

QJsonObject ClientHandler::handleRsaDecrypt(const QJsonObject &req) {
    QString b64  = req["encrypted"].toString();
    QString keys = req["keys"].toString();

    if (b64.isEmpty() || keys.isEmpty())
        return errorResp("Не переданы зашифрованный текст или ключи");

    RSA::KeyPair kp  = RSA::keyPairFromString(keys);
    QByteArray   enc = QByteArray::fromBase64(b64.toLatin1());
    QByteArray   dec = RSA::decrypt(enc, kp.n, kp.d);

    QJsonObject resp = okResp();
    resp["decrypted"] = QString::fromUtf8(dec);
    return resp;
}

// ──────────────────────── SHA-1 ───────────────────────────────

QJsonObject ClientHandler::handleSha1(const QJsonObject &req) {
    QString text = req["text"].toString();
    QJsonObject resp = okResp();
    resp["hash"] = SHA1::hash(text);
    return resp;
}

// ─────────────────────── Метод Ньютона ───────────────────────

QJsonObject ClientHandler::handleNewton(const QJsonObject &req) {
    // Клиент передаёт уравнение как индекс преднастроенной функции
    // (расширить позже на произвольный ввод через exprtk или аналог)
    int    funcId = req["func_id"].toInt(0);
    double x0     = req["x0"].toDouble(1.0);

    NewtonSolver solver(1e-9, 1000);

    // Набор тестовых уравнений
    switch (funcId) {
    case 0: // x^3 - x - 2 = 0,  корень ~1.5214
        solver.setFunction  ([](double x){ return x*x*x - x - 2; });
        solver.setDerivative([](double x){ return 3*x*x - 1; });
        break;
    case 1: // cos(x) - x = 0,  корень ~0.7391
        solver.setFunction  ([](double x){ return std::cos(x) - x; });
        solver.setDerivative([](double x){ return -std::sin(x) - 1; });
        break;
    case 2: // e^x - 3x = 0,  корень ~0.6190
        solver.setFunction  ([](double x){ return std::exp(x) - 3*x; });
        solver.setDerivative([](double x){ return std::exp(x) - 3; });
        break;
    default:
        return errorResp("Неизвестный func_id");
    }

    bool ok;
    auto iters = solver.solveDetailed(x0, &ok);

    QJsonArray arr;
    for (auto &it : iters) {
        QJsonObject o;
        o["x"]  = it.x;
        o["fx"] = it.fx;
        arr.append(o);
    }

    QJsonObject resp = ok ? okResp() : errorResp("Метод не сошёлся");
    resp["root"]       = iters.isEmpty() ? x0 : iters.last().x;
    resp["iterations"] = arr;
    return resp;
}

// ────────────────────── Стеганография ────────────────────────

QJsonObject ClientHandler::handleEmbedAudio(const QJsonObject &req) {
    QString wavB64  = req["wav_base64"].toString();
    QString message = req["message"].toString();

    if (wavB64.isEmpty())  return errorResp("WAV-файл не передан");
    if (message.isEmpty()) return errorResp("Сообщение пустое");

    // Сохраняем временный файл
    QByteArray wavData = QByteArray::fromBase64(wavB64.toLatin1());
    QString tmpIn  = "/tmp/steg_in.wav";
    QString tmpOut = "/tmp/steg_out.wav";

    QFile f(tmpIn);
    f.open(QIODevice::WriteOnly);
    f.write(wavData);
    f.close();

    AudioSteganography steg;
    if (!steg.loadWav(tmpIn)) return errorResp(steg.lastError());
    if (!steg.embedMessage(message, tmpOut)) return errorResp(steg.lastError());

    QFile out(tmpOut);
    out.open(QIODevice::ReadOnly);
    QByteArray result = out.readAll();
    out.close();

    QJsonObject resp = okResp("Сообщение внедрено");
    resp["wav_base64"] = QString::fromLatin1(result.toBase64());
    return resp;
}

QJsonObject ClientHandler::handleExtractAudio(const QJsonObject &req) {
    QString wavB64 = req["wav_base64"].toString();
    if (wavB64.isEmpty()) return errorResp("WAV-файл не передан");

    QByteArray wavData = QByteArray::fromBase64(wavB64.toLatin1());
    QString tmpIn = "/tmp/steg_extract.wav";
    QFile f(tmpIn);
    f.open(QIODevice::WriteOnly);
    f.write(wavData);
    f.close();

    AudioSteganography steg;
    if (!steg.loadWav(tmpIn)) return errorResp(steg.lastError());

    bool ok;
    QString msg = steg.extractMessage(&ok);
    if (!ok) return errorResp("Не удалось извлечь сообщение: " + steg.lastError());

    QJsonObject resp = okResp();
    resp["message"] = msg;
    return resp;
}

// ────────────────────── Админ ─────────────────────────────────

QJsonObject ClientHandler::handleGetUsers(const QJsonObject &) {
    if (!isAdmin()) return errorResp("Доступ запрещён");

    QJsonArray arr;
    for (const User &u : Database::getInstance()->allUsers())
        arr.append(u.toJson());

    QJsonObject resp = okResp();
    resp["users"] = arr;
    return resp;
}

QJsonObject ClientHandler::handleDeleteUser(const QJsonObject &req) {
    if (!isAdmin()) return errorResp("Доступ запрещён");

    QString login = req["login"].toString();
    if (login == "admin") return errorResp("Нельзя удалить администратора");

    Database::getInstance()->deleteUser(login);
    return okResp("Пользователь удалён");
}

QJsonObject ClientHandler::handleSetRole(const QJsonObject &req) {
    if (!isAdmin()) return errorResp("Доступ запрещён");

    QString login = req["login"].toString();
    QString role  = req["role"].toString();

    if (role != "user" && role != "admin")
        return errorResp("Недопустимая роль (user/admin)");

    Database::getInstance()->updateRole(login, role);
    return okResp("Роль обновлена");
}

// ──────────────────────── хелперы ─────────────────────────────

QJsonObject ClientHandler::errorResp(const QString &msg) {
    return { {"status", "error"}, {"message", msg} };
}

QJsonObject ClientHandler::okResp(const QString &msg) {
    return { {"status", "ok"}, {"message", msg} };
}
