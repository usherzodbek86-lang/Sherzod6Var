#pragma once
#include "user.h"
#include <QtSql/QSqlDatabase>
#include <QList>

/**
 * @brief Singleton-обёртка над SQLite базой данных.
 *
 * Хранит пользователей (login, passwordHash, role).
 * Первый администратор создаётся автоматически при инициализации.
 */
class Database {
public:
    /** @brief Получить единственный экземпляр. */
    static Database *getInstance();

    /** @brief Инициализировать БД (создать таблицы). */
    bool init(const QString &dbPath = "var6.db");

    /** @brief Найти пользователя по логину. */
    User getUser(const QString &login) const;

    /** @brief Сохранить нового пользователя. */
    bool saveUser(const User &user);

    /** @brief Обновить роль пользователя. */
    bool updateRole(const QString &login, const QString &role);

    /** @brief Удалить пользователя. */
    bool deleteUser(const QString &login);

    /** @brief Получить список всех пользователей. */
    QList<User> allUsers() const;

private:
    Database() = default;
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    static Database *s_instance;
    QSqlDatabase     m_db;
};
