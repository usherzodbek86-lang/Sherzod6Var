#include "server.h"
#include "database.h"
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    // Инициализируем БД
    if (!Database::getInstance()->init()) {
        qCritical() << "Ошибка инициализации базы данных";
        return 1;
    }

    // Запускаем сервер
    Server server(9000);
    if (!server.start())
        return 1;

    return app.exec();
}
