#pragma once
#include <QString>
#include <QJsonObject>

struct User {
    int     id           = 0;
    QString login;
    QString passwordHash;   // SHA-1 хеш пароля
    QString role;           // "user" | "admin"

    bool isValid() const { return id > 0; }

    QJsonObject toJson() const {
        QJsonObject o;
        o["id"]    = id;
        o["login"] = login;
        o["role"]  = role;
        return o;
    }
};
