#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

/**
 * @brief Диалог авторизации и регистрации.
 */
class LoginWindow : public QDialog {
    Q_OBJECT
public:
    explicit LoginWindow(QWidget *parent = nullptr);

signals:
    void loginSuccess(const QString &login, const QString &role);

private slots:
    void onLogin();
    void onRegister();
    void onResponse(const QJsonObject &resp);

private:
    void setupUi();
    void setStatus(const QString &msg, bool error = false);

    QLineEdit  *m_loginEdit;
    QLineEdit  *m_passEdit;
    QLineEdit  *m_hostEdit;
    QPushButton *m_loginBtn;
    QPushButton *m_registerBtn;
    QLabel      *m_statusLabel;

    bool m_waitingLogin    = false;
    bool m_waitingRegister = false;
};
