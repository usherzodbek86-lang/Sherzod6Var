#include "functionpanel.h"
#include "networkmanager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QFileDialog>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QFileInfo>

// ─────────────────── конструктор ─────────────────────────────

FunctionPanel::FunctionPanel(QWidget *parent) : QWidget(parent) {
    setupUi();
    connect(NetworkManager::getInstance(), &NetworkManager::responseReceived,
            this, &FunctionPanel::onResponse);
}

void FunctionPanel::setupUi() {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);

    auto *tabs = new QTabWidget(this);
    tabs->addTab(buildRsaTab(),    "🔐 RSA");
    tabs->addTab(buildSha1Tab(),   "# SHA-1");
    tabs->addTab(buildNewtonTab(), "∫ Ньютон");
    tabs->addTab(buildStegoTab(),  "🎵 Стеганография");

    root->addWidget(tabs);
}

// ──────────────────── RSA вкладка ─────────────────────────────

QWidget *FunctionPanel::buildRsaTab() {
    auto *w    = new QWidget;
    auto *root = new QVBoxLayout(w);
    root->setSpacing(8);

    auto *inputGroup = new QGroupBox("Текст для шифрования");
    auto *ig = new QVBoxLayout(inputGroup);
    m_rsaInput = new QTextEdit;
    m_rsaInput->setPlaceholderText("Введите текст...");
    m_rsaInput->setMaximumHeight(80);
    ig->addWidget(m_rsaInput);

    auto *btnRow = new QHBoxLayout;
    auto *encBtn = new QPushButton("Зашифровать");
    auto *decBtn = new QPushButton("Расшифровать");
    btnRow->addWidget(encBtn);
    btnRow->addWidget(decBtn);

    auto *encGroup = new QGroupBox("Зашифрованный текст (Base64)");
    auto *eg = new QVBoxLayout(encGroup);
    m_rsaEncrypted = new QTextEdit;
    m_rsaEncrypted->setMaximumHeight(80);
    eg->addWidget(m_rsaEncrypted);

    auto *keysGroup = new QGroupBox("Ключи (JSON) — сохраните для дешифровки!");
    auto *kg = new QVBoxLayout(keysGroup);
    m_rsaKeys = new QTextEdit;
    m_rsaKeys->setMaximumHeight(60);
    kg->addWidget(m_rsaKeys);

    m_rsaStatus = new QLabel;
    m_rsaStatus->setAlignment(Qt::AlignCenter);

    root->addWidget(inputGroup);
    root->addLayout(btnRow);
    root->addWidget(encGroup);
    root->addWidget(keysGroup);
    root->addWidget(m_rsaStatus);
    root->addStretch();

    connect(encBtn, &QPushButton::clicked, this, &FunctionPanel::onRsaEncrypt);
    connect(decBtn, &QPushButton::clicked, this, &FunctionPanel::onRsaDecrypt);
    return w;
}

// ──────────────────── SHA-1 вкладка ───────────────────────────

QWidget *FunctionPanel::buildSha1Tab() {
    auto *w    = new QWidget;
    auto *root = new QVBoxLayout(w);
    root->setSpacing(12);

    auto *form = new QFormLayout;
    m_sha1Input = new QLineEdit;
    m_sha1Input->setPlaceholderText("Введите строку...");
    form->addRow("Строка:", m_sha1Input);
    root->addLayout(form);

    auto *btn = new QPushButton("Вычислить хеш SHA-1");
    root->addWidget(btn);

    auto *resultGroup = new QGroupBox("Результат");
    auto *rg = new QVBoxLayout(resultGroup);
    m_sha1Result = new QLabel("—");
    m_sha1Result->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_sha1Result->setWordWrap(true);
    m_sha1Result->setStyleSheet("font-family: monospace; font-size: 14px; padding: 4px;");
    rg->addWidget(m_sha1Result);
    root->addWidget(resultGroup);
    root->addStretch();

    connect(btn, &QPushButton::clicked, this, &FunctionPanel::onSha1Hash);
    return w;
}

// ──────────────────── Ньютон вкладка ──────────────────────────

QWidget *FunctionPanel::buildNewtonTab() {
    auto *w    = new QWidget;
    auto *root = new QVBoxLayout(w);
    root->setSpacing(8);

    auto *form = new QFormLayout;
    m_newtonFunc = new QComboBox;
    m_newtonFunc->addItem("x³ − x − 2 = 0  (корень ≈ 1.5214)",  0);
    m_newtonFunc->addItem("cos(x) − x = 0  (корень ≈ 0.7391)",   1);
    m_newtonFunc->addItem("eˣ − 3x = 0     (корень ≈ 0.6190)",   2);
    m_newtonX0 = new QLineEdit("1.0");
    form->addRow("Уравнение:", m_newtonFunc);
    form->addRow("Нач. приближение x₀:", m_newtonX0);
    root->addLayout(form);

    auto *btn = new QPushButton("Решить методом Ньютона");
    root->addWidget(btn);

    m_newtonRoot = new QLabel("Корень: —");
    m_newtonRoot->setStyleSheet("font-weight: bold; font-size: 14px; padding: 4px;");
    root->addWidget(m_newtonRoot);

    auto *iterGroup = new QGroupBox("История итераций");
    auto *ig = new QVBoxLayout(iterGroup);
    m_newtonTable = new ResultTable;
    ig->addWidget(m_newtonTable);
    root->addWidget(iterGroup);

    connect(btn, &QPushButton::clicked, this, &FunctionPanel::onNewtonSolve);
    return w;
}

// ──────────────────── Стеганография вкладка ───────────────────

QWidget *FunctionPanel::buildStegoTab() {
    auto *w    = new QWidget;
    auto *root = new QVBoxLayout(w);
    root->setSpacing(8);

    // WAV-файл
    auto *fileGroup = new QGroupBox("WAV-файл");
    auto *fg = new QHBoxLayout(fileGroup);
    m_stegoFile = new QLabel("Файл не выбран");
    auto *browseBtn = new QPushButton("Обзор...");
    fg->addWidget(m_stegoFile, 1);
    fg->addWidget(browseBtn);
    root->addWidget(fileGroup);

    // Сообщение
    auto *msgGroup = new QGroupBox("Сообщение");
    auto *mg = new QVBoxLayout(msgGroup);
    m_stegoMessage = new QLineEdit;
    m_stegoMessage->setPlaceholderText("Текст для внедрения...");
    mg->addWidget(m_stegoMessage);
    root->addWidget(msgGroup);

    // Кнопки
    auto *btnRow = new QHBoxLayout;
    auto *embedBtn   = new QPushButton("Внедрить сообщение");
    auto *extractBtn = new QPushButton("Извлечь сообщение");
    btnRow->addWidget(embedBtn);
    btnRow->addWidget(extractBtn);
    root->addLayout(btnRow);

    m_stegoStatus = new QLabel;
    m_stegoStatus->setWordWrap(true);
    m_stegoStatus->setAlignment(Qt::AlignCenter);
    root->addWidget(m_stegoStatus);
    root->addStretch();

    connect(browseBtn, &QPushButton::clicked, this, [this]{
        m_stegoFilePath = QFileDialog::getOpenFileName(
            this, "Выбрать WAV-файл", {}, "WAV Files (*.wav)");
        if (!m_stegoFilePath.isEmpty())
            m_stegoFile->setText(QFileInfo(m_stegoFilePath).fileName());
    });
    connect(embedBtn,   &QPushButton::clicked, this, &FunctionPanel::onEmbedAudio);
    connect(extractBtn, &QPushButton::clicked, this, &FunctionPanel::onExtractAudio);
    return w;
}

// ──────────────────── слоты ───────────────────────────────────

void FunctionPanel::onRsaEncrypt() {
    QString text = m_rsaInput->toPlainText().trimmed();
    if (text.isEmpty()) { m_rsaStatus->setText("Введите текст!"); return; }
    m_pendingAction = "rsa_encrypt";
    QJsonObject req;
    req["action"] = "rsa_encrypt";
    req["text"]   = text;
    NetworkManager::getInstance()->sendRequest(req);
    m_rsaStatus->setText("Шифрование...");
}

void FunctionPanel::onRsaDecrypt() {
    QString enc  = m_rsaEncrypted->toPlainText().trimmed();
    QString keys = m_rsaKeys->toPlainText().trimmed();
    if (enc.isEmpty() || keys.isEmpty()) {
        m_rsaStatus->setText("Заполните поля зашифрованного текста и ключей");
        return;
    }
    m_pendingAction = "rsa_decrypt";
    QJsonObject req;
    req["action"]    = "rsa_decrypt";
    req["encrypted"] = enc;
    req["keys"]      = keys;
    NetworkManager::getInstance()->sendRequest(req);
    m_rsaStatus->setText("Дешифровка...");
}

void FunctionPanel::onSha1Hash() {
    QString text = m_sha1Input->text().trimmed();
    m_pendingAction = "sha1";
    QJsonObject req;
    req["action"] = "sha1";
    req["text"]   = text;
    NetworkManager::getInstance()->sendRequest(req);
}

void FunctionPanel::onNewtonSolve() {
    m_pendingAction = "newton";
    QJsonObject req;
    req["action"]  = "newton";
    req["func_id"] = m_newtonFunc->currentData().toInt();
    req["x0"]      = m_newtonX0->text().toDouble();
    NetworkManager::getInstance()->sendRequest(req);
}

void FunctionPanel::onEmbedAudio() {
    if (m_stegoFilePath.isEmpty()) {
        m_stegoStatus->setText("Выберите WAV-файл!");
        return;
    }
    QString msg = m_stegoMessage->text().trimmed();
    if (msg.isEmpty()) { m_stegoStatus->setText("Введите сообщение!"); return; }

    QFile f(m_stegoFilePath);
    if (!f.open(QIODevice::ReadOnly)) {
        m_stegoStatus->setText("Не удалось открыть файл");
        return;
    }
    m_lastWavData = f.readAll();
    f.close();

    m_pendingAction = "embed_audio";
    QJsonObject req;
    req["action"]     = "embed_audio";
    req["wav_base64"] = QString::fromLatin1(m_lastWavData.toBase64());
    req["message"]    = msg;
    NetworkManager::getInstance()->sendRequest(req);
    m_stegoStatus->setText("Внедрение...");
}

void FunctionPanel::onExtractAudio() {
    if (m_stegoFilePath.isEmpty()) {
        m_stegoStatus->setText("Выберите WAV-файл!");
        return;
    }
    QFile f(m_stegoFilePath);
    if (!f.open(QIODevice::ReadOnly)) {
        m_stegoStatus->setText("Не удалось открыть файл");
        return;
    }
    m_lastWavData = f.readAll();
    f.close();

    m_pendingAction = "extract_audio";
    QJsonObject req;
    req["action"]     = "extract_audio";
    req["wav_base64"] = QString::fromLatin1(m_lastWavData.toBase64());
    NetworkManager::getInstance()->sendRequest(req);
    m_stegoStatus->setText("Извлечение...");
}

// ──────────────────── обработка ответов ───────────────────────

void FunctionPanel::onResponse(const QJsonObject &resp) {
    QString status = resp["status"].toString();
    bool ok = (status == "ok");

    if (m_pendingAction == "rsa_encrypt") {
        m_pendingAction.clear();
        if (ok) {
            m_rsaEncrypted->setPlainText(resp["encrypted"].toString());
            m_rsaKeys->setPlainText(resp["keys"].toString());
            m_rsaStatus->setStyleSheet("color: green;");
            m_rsaStatus->setText("Зашифровано успешно. Сохраните ключи!");
        } else {
            m_rsaStatus->setStyleSheet("color: red;");
            m_rsaStatus->setText(resp["message"].toString());
        }
    }
    else if (m_pendingAction == "rsa_decrypt") {
        m_pendingAction.clear();
        if (ok) {
            m_rsaInput->setPlainText(resp["decrypted"].toString());
            m_rsaStatus->setStyleSheet("color: green;");
            m_rsaStatus->setText("Расшифровано успешно.");
        } else {
            m_rsaStatus->setStyleSheet("color: red;");
            m_rsaStatus->setText(resp["message"].toString());
        }
    }
    else if (m_pendingAction == "sha1") {
        m_pendingAction.clear();
        if (ok)
            m_sha1Result->setText(resp["hash"].toString());
        else
            m_sha1Result->setText("Ошибка: " + resp["message"].toString());
    }
    else if (m_pendingAction == "newton") {
        m_pendingAction.clear();
        if (ok) {
            double root = resp["root"].toDouble();
            m_newtonRoot->setText(QString("Корень: x = %1").arg(root, 0, 'g', 10));

            QJsonArray iters = resp["iterations"].toArray();
            QList<QPair<QString,QString>> rows;
            rows.append(qMakePair(QString("Итерация"), QString("x")));
            for (int i = 0; i < iters.size(); ++i) {
                QJsonObject it = iters[i].toObject();
                rows.append(qMakePair(
                    QString("Итерация %1").arg(i + 1),
                    QString("x=%1  f(x)=%2")
                        .arg(it["x"].toDouble(), 0, 'g', 8)
                        .arg(it["fx"].toDouble(), 0, 'g', 8)
                    ));
            }
            m_newtonTable->showResult(rows);
        } else {
            m_newtonRoot->setText("Ошибка: " + resp["message"].toString());
        }
    }
    else if (m_pendingAction == "embed_audio") {
        m_pendingAction.clear();
        if (ok) {
            QString outPath = QFileDialog::getSaveFileName(
                this, "Сохранить WAV с сообщением", {}, "WAV Files (*.wav)");
            if (!outPath.isEmpty()) {
                QByteArray data = QByteArray::fromBase64(
                    resp["wav_base64"].toString().toLatin1());
                QFile out(outPath);
                out.open(QIODevice::WriteOnly);
                out.write(data);
                out.close();
                m_stegoStatus->setStyleSheet("color: green;");
                m_stegoStatus->setText("Сохранено: " + outPath);
            }
        } else {
            m_stegoStatus->setStyleSheet("color: red;");
            m_stegoStatus->setText(resp["message"].toString());
        }
    }
    else if (m_pendingAction == "extract_audio") {
        m_pendingAction.clear();
        if (ok) {
            m_stegoMessage->setText(resp["message"].toString());
            m_stegoStatus->setStyleSheet("color: green;");
            m_stegoStatus->setText("Сообщение извлечено успешно.");
        } else {
            m_stegoStatus->setStyleSheet("color: red;");
            m_stegoStatus->setText(resp["message"].toString());
        }
    }
}