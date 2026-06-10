#include "loginwindow.h"
#include "networkmanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QJsonObject>
#include <QMessageBox>

LoginWindow::LoginWindow(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Вход — Var6");
    setFixedSize(340, 280);
    setupUi();

    connect(NetworkManager::getInstance(), &NetworkManager::responseReceived,
            this, &LoginWindow::onResponse);
}

void LoginWindow::setupUi() {
    auto *root = new QVBoxLayout(this);
    root->setSpacing(12);
    root->setContentsMargins(20, 20, 20, 20);

    // Заголовок
    auto *title = new QLabel("<b>Система шифрования и стеганографии</b>");
    title->setAlignment(Qt::AlignCenter);
    root->addWidget(title);

    // Поля
    auto *form = new QFormLayout();
    m_hostEdit  = new QLineEdit("127.0.0.1");
    m_loginEdit = new QLineEdit();
    m_passEdit  = new QLineEdit();
    m_passEdit->setEchoMode(QLineEdit::Password);
    m_loginEdit->setPlaceholderText("логин");
    m_passEdit->setPlaceholderText("пароль");
    form->addRow("Сервер:", m_hostEdit);
    form->addRow("Логин:", m_loginEdit);
    form->addRow("Пароль:", m_passEdit);
    root->addLayout(form);

    // Кнопки
    auto *btnRow = new QHBoxLayout();
    m_loginBtn    = new QPushButton("Войти");
    m_registerBtn = new QPushButton("Регистрация");
    btnRow->addWidget(m_loginBtn);
    btnRow->addWidget(m_registerBtn);
    root->addLayout(btnRow);

    // Статус
    m_statusLabel = new QLabel();
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setWordWrap(true);
    root->addWidget(m_statusLabel);

    connect(m_loginBtn,    &QPushButton::clicked, this, &LoginWindow::onLogin);
    connect(m_registerBtn, &QPushButton::clicked, this, &LoginWindow::onRegister);
}

void LoginWindow::setStatus(const QString &msg, bool error) {
    m_statusLabel->setText(msg);
    m_statusLabel->setStyleSheet(error
        ? "color: #c0392b; font-weight: bold;"
        : "color: #27ae60; font-weight: bold;");
}

void LoginWindow::onLogin() {
    auto *nm = NetworkManager::getInstance();
    if (!nm->isConnected()) {
        if (!nm->connectToServer(m_hostEdit->text())) {
            setStatus("Нет подключения к серверу", true);
            return;
        }
    }
    m_waitingLogin = true;
    nm->sendRequest({
        {"action",   "login"},
        {"login",    m_loginEdit->text().trimmed()},
        {"password", m_passEdit->text()}
    });
    m_loginBtn->setEnabled(false);
    setStatus("Подождите...");
}

void LoginWindow::onRegister() {
    auto *nm = NetworkManager::getInstance();
    if (!nm->isConnected()) {
        if (!nm->connectToServer(m_hostEdit->text())) {
            setStatus("Нет подключения к серверу", true);
            return;
        }
    }
    m_waitingRegister = true;
    nm->sendRequest({
        {"action",   "register"},
        {"login",    m_loginEdit->text().trimmed()},
        {"password", m_passEdit->text()}
    });
    m_registerBtn->setEnabled(false);
    setStatus("Регистрация...");
}

void LoginWindow::onResponse(const QJsonObject &resp) {
    m_loginBtn->setEnabled(true);
    m_registerBtn->setEnabled(true);

    QString status = resp["status"].toString();

    if (m_waitingLogin) {
        m_waitingLogin = false;
        if (status == "ok") {
            QJsonObject user = resp["user"].toObject();
            QString login = user["login"].toString();
            QString role  = user["role"].toString();
            NetworkManager::getInstance()->setCurrentUser(login, role);
            emit loginSuccess(login, role);
            accept();
        } else {
            setStatus(resp["message"].toString(), true);
        }
    } else if (m_waitingRegister) {
        m_waitingRegister = false;
        if (status == "ok")
            setStatus("Регистрация успешна! Теперь войдите.");
        else
            setStatus(resp["message"].toString(), true);
    }
}
