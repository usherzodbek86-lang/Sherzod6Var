#include "server.h"
#include "clienthandler.h"
#include <QDebug>

Server::Server(quint16 port, QObject *parent)
    : QObject(parent)
    , m_server(new QTcpServer(this))
    , m_port(port)
{
    connect(m_server, &QTcpServer::newConnection,
            this,     &Server::onNewConnection);
}

bool Server::start() {
    if (!m_server->listen(QHostAddress::Any, m_port)) {
        qCritical() << "Не удалось запустить сервер на порту" << m_port
                    << ":" << m_server->errorString();
        return false;
    }
    qInfo() << "Сервер запущен на порту" << m_port;
    return true;
}

void Server::stop() {
    m_server->close();
    qInfo() << "Сервер остановлен";
}

void Server::onNewConnection() {
    while (m_server->hasPendingConnections()) {
        QTcpSocket *socket = m_server->nextPendingConnection();
        qInfo() << "Новое подключение:" << socket->peerAddress().toString();

        auto *handler = new ClientHandler(socket, this);
        connect(handler, &ClientHandler::disconnected,
                this,    &Server::onClientDisconnected);
        m_clients.append(handler);
    }
}

void Server::onClientDisconnected(ClientHandler *handler) {
    qInfo() << "Клиент отключился";
    m_clients.removeOne(handler);
    handler->deleteLater();
}
