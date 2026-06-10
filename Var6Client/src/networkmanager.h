#pragma once
#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>

/**
 * @brief Singleton для сетевого взаимодействия с сервером.
 *
 * Хранит одно TCP-соединение на всё время работы клиента.
 * Отправляет JSON-запросы, испускает сигнал при получении ответа.
 */
class NetworkManager : public QObject {
    Q_OBJECT
public:
    static NetworkManager *getInstance();

    /** @brief Подключиться к серверу. */
    bool connectToServer(const QString &host = "127.0.0.1", quint16 port = 9000);

    /** @brief Отправить JSON-запрос. */
    void sendRequest(const QJsonObject &req);

    bool isConnected() const;

    // Данные авторизованного пользователя
    QString currentLogin() const { return m_login; }
    QString currentRole()  const { return m_role;  }
    void    setCurrentUser(const QString &login, const QString &role);

signals:
    void responseReceived(const QJsonObject &resp);
    void connectionLost();

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    NetworkManager();
    static NetworkManager *s_instance;

    QTcpSocket *m_socket;
    QByteArray  m_buffer;
    QString     m_login;
    QString     m_role;
};
