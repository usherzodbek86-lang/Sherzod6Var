#pragma once
#include <QWidget>
#include <QJsonObject>
#include <QLabel>        // ← добавить
#include "resulttable.h"
/**
 * @brief Панель администратора: список пользователей, удаление, смена роли.
 * Отображается только при role == "admin".
 */
class AdminPanel : public QWidget {
    Q_OBJECT
public:
    explicit AdminPanel(QWidget *parent = nullptr);

    void refresh();

private slots:
    void onResponse(const QJsonObject &resp);
    void onDeleteUser();
    void onSetRole();

private:
    ResultTable *m_table;
    QLabel      *m_status;
    QString      m_pendingAction;
};
