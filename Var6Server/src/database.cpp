#include "database.h"
#include "sha1.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>

Database *Database::s_instance = nullptr;

Database *Database::getInstance() {
    if (!s_instance)
        s_instance = new Database();
    return s_instance;
}

bool Database::init(const QString &dbPath) {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);

    if (!m_db.open()) {
        qCritical() << "DB open error:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery q(m_db);
    q.exec(R"(
        CREATE TABLE IF NOT EXISTS users (
            id           INTEGER PRIMARY KEY AUTOINCREMENT,
            login        TEXT    UNIQUE NOT NULL,
            passwordHash TEXT    NOT NULL,
            role         TEXT    NOT NULL DEFAULT 'user'
        )
    )");

    // Создаём admin по умолчанию, если таблица пустая
    q.exec("SELECT COUNT(*) FROM users");
    if (q.next() && q.value(0).toInt() == 0) {
        User admin;
        admin.login        = "admin";
        admin.passwordHash = SHA1::hash("admin123");
        admin.role         = "admin";
        saveUser(admin);
        qInfo() << "Создан администратор: login=admin, password=admin123";
    }

    return true;
}

User Database::getUser(const QString &login) const {
    QSqlQuery q(m_db);
    q.prepare("SELECT id, login, passwordHash, role FROM users WHERE login = ?");
    q.addBindValue(login);
    q.exec();

    if (q.next()) {
        return { q.value(0).toInt(),
                 q.value(1).toString(),
                 q.value(2).toString(),
                 q.value(3).toString() };
    }
    return {};
}

bool Database::saveUser(const User &user) {
    QSqlQuery q(m_db);
    q.prepare("INSERT INTO users (login, passwordHash, role) VALUES (?, ?, ?)");
    q.addBindValue(user.login);
    q.addBindValue(user.passwordHash);
    q.addBindValue(user.role.isEmpty() ? "user" : user.role);
    if (!q.exec()) {
        qWarning() << "saveUser error:" << q.lastError().text();
        return false;
    }
    return true;
}

bool Database::updateRole(const QString &login, const QString &role) {
    QSqlQuery q(m_db);
    q.prepare("UPDATE users SET role = ? WHERE login = ?");
    q.addBindValue(role);
    q.addBindValue(login);
    return q.exec();
}

bool Database::deleteUser(const QString &login) {
    QSqlQuery q(m_db);
    q.prepare("DELETE FROM users WHERE login = ?");
    q.addBindValue(login);
    return q.exec();
}

QList<User> Database::allUsers() const {
    QList<User> list;
    QSqlQuery q("SELECT id, login, passwordHash, role FROM users", m_db);
    while (q.next()) {
        list.append({ q.value(0).toInt(),
                      q.value(1).toString(),
                      q.value(2).toString(),
                      q.value(3).toString() });
    }
    return list;
}
