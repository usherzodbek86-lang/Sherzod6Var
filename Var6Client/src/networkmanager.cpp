#include "networkmanager.h"
#include <QJsonDocument>
#include <QDebug>

NetworkManager *NetworkManager::s_instance = nullptr;

NetworkManager::NetworkManager()
    : QObject(nullptr)
    , m_socket(new QTcpSocket(this))
{
    connect(m_socket, &QTcpSocket::readyRead,
            this,     &NetworkManager::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected,
            this,     &NetworkManager::onDisconnected);
}

NetworkManager *NetworkManager::getInstance() {
    if (!s_instance)
        s_instance = new NetworkManager();
    return s_instance;
}

bool NetworkManager::connectToServer(const QString &host, quint16 port) {
    m_socket->connectToHost(host, port);
    return m_socket->waitForConnected(3000);
}

void NetworkManager::sendRequest(const QJsonObject &req) {
    if (!isConnected()) {
        qWarning() << "Нет подключения к серверу";
        return;
    }
    QByteArray data = QJsonDocument(req).toJson(QJsonDocument::Compact) + "\n";
    m_socket->write(data);
    m_socket->flush();
}

bool NetworkManager::isConnected() const {
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

void NetworkManager::setCurrentUser(const QString &login, const QString &role) {
    m_login = login;
    m_role  = role;
}

void NetworkManager::onReadyRead() {
    m_buffer += m_socket->readAll();
    while (m_buffer.contains('\n')) {
        int idx  = m_buffer.indexOf('\n');
        QByteArray line = m_buffer.left(idx).trimmed();
        m_buffer = m_buffer.mid(idx + 1);
        if (line.isEmpty()) continue;

        QJsonDocument doc = QJsonDocument::fromJson(line);
        if (doc.isObject())
            emit responseReceived(doc.object());
    }
}

void NetworkManager::onDisconnected() {
    emit connectionLost();
}
