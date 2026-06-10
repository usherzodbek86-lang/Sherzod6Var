#pragma once
#include <QTableWidget>
#include <QStringList>

/**
 * @brief Таблица для отображения результатов операций.
 */
class ResultTable : public QTableWidget {
    Q_OBJECT
public:
    explicit ResultTable(QWidget *parent = nullptr);

    /** @brief Показать одну строку результата: список пар "ключ-значение". */
    void showResult(const QList<QPair<QString,QString>> &rows);

    /** @brief Показать список пользователей (для панели администратора). */
    void showUsers(const QJsonArray &users);

    void clearAll();

private:
    void setupStyle();
};
