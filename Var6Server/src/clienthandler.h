#pragma once
#include "user.h"
#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>

/**
 * @brief Обработчик одного подключённого клиента.
 *
 * Создаётся сервером для каждого нового соединения.
 * Разбирает JSON-запросы и отправляет JSON-ответы.
 *
 * Протокол: каждое сообщение — одна строка JSON + '\\n'.
 *
 * Поля запроса:
 *   { "action": "...", "login": "...", "password": "...", ... }
 *
 * Поля ответа:
 *   { "status": "ok"|"error", "message": "...", ... }
 */
class ClientHandler : public QObject {
    Q_OBJECT
public:
    explicit ClientHandler(QTcpSocket *socket, QObject *parent = nullptr);

signals:
    void disconnected(ClientHandler *self);

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    void parseRequest(const QByteArray &data);
    void sendResponse(const QJsonObject &obj);

    // --- обработчики действий ---
    QJsonObject handleRegister(const QJsonObject &req);
    QJsonObject handleLogin   (const QJsonObject &req);

    QJsonObject handleRsaEncrypt (const QJsonObject &req);
    QJsonObject handleRsaDecrypt (const QJsonObject &req);
    QJsonObject handleSha1       (const QJsonObject &req);
    QJsonObject handleNewton     (const QJsonObject &req);
    QJsonObject handleEmbedAudio (const QJsonObject &req);
    QJsonObject handleExtractAudio(const QJsonObject &req);

    // --- только для admin ---
    QJsonObject handleGetUsers   (const QJsonObject &req);
    QJsonObject handleDeleteUser (const QJsonObject &req);
    QJsonObject handleSetRole    (const QJsonObject &req);

    // --- вспомогательные ---
    bool        isAdmin() const { return m_currentUser.role == "admin"; }
    QJsonObject errorResp(const QString &msg);
    QJsonObject okResp(const QString &msg = "ok");

    QTcpSocket *m_socket;
    User        m_currentUser;   // авторизованный пользователь (id == 0 = гость)
    QByteArray  m_buffer;        // буфер для неполных пакетов
};
