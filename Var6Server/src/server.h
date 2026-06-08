#pragma once
#include <QObject>
#include <QTcpServer>
#include <QList>

class ClientHandler;

/**
 * @brief TCP-сервер. Принимает входящие подключения,
 *        создаёт ClientHandler для каждого клиента.
 */
class Server : public QObject {
    Q_OBJECT
public:
    explicit Server(quint16 port = 9000, QObject *parent = nullptr);

    bool start();
    void stop();

private slots:
    void onNewConnection();
    void onClientDisconnected(ClientHandler *handler);

private:
    QTcpServer          *m_server;
    quint16              m_port;
    QList<ClientHandler*> m_clients;
};
