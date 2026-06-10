#include "adminpanel.h"
#include "networkmanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>
#include <QJsonArray>

AdminPanel::AdminPanel(QWidget *parent) : QWidget(parent) {
    auto *root = new QVBoxLayout(this);
    root->setSpacing(8);

    auto *title = new QLabel("<b>Панель администратора</b>");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 15px; color: #c0392b; padding: 6px;");
    root->addWidget(title);

    m_table = new ResultTable(this);
    root->addWidget(m_table);

    auto *btnRow = new QHBoxLayout;
    auto *refreshBtn = new QPushButton("Обновить список");
    auto *deleteBtn  = new QPushButton("Удалить пользователя");
    auto *roleBtn    = new QPushButton("Изменить роль");
    deleteBtn->setStyleSheet("color: #c0392b; font-weight: bold;");
    btnRow->addWidget(refreshBtn);
    btnRow->addWidget(deleteBtn);
    btnRow->addWidget(roleBtn);
    root->addLayout(btnRow);

    m_status = new QLabel;
    m_status->setAlignment(Qt::AlignCenter);
    root->addWidget(m_status);

    connect(refreshBtn, &QPushButton::clicked, this, &AdminPanel::refresh);
    connect(deleteBtn,  &QPushButton::clicked, this, &AdminPanel::onDeleteUser);
    connect(roleBtn,    &QPushButton::clicked, this, &AdminPanel::onSetRole);

    connect(NetworkManager::getInstance(), &NetworkManager::responseReceived,
            this, &AdminPanel::onResponse);
}

void AdminPanel::refresh() {
    m_pendingAction = "get_users";
    NetworkManager::getInstance()->sendRequest({{"action","get_users"}});
}

void AdminPanel::onDeleteUser() {
    int row = m_table->currentRow();
    if (row < 0) { m_status->setText("Выберите пользователя в таблице"); return; }
    QString login = m_table->item(row, 1)->text();

    if (QMessageBox::question(this, "Удаление",
            "Удалить пользователя \"" + login + "\"?") != QMessageBox::Yes) return;

    m_pendingAction = "delete_user";
    NetworkManager::getInstance()->sendRequest({{"action","delete_user"},{"login",login}});
}

void AdminPanel::onSetRole() {
    int row = m_table->currentRow();
    if (row < 0) { m_status->setText("Выберите пользователя в таблице"); return; }
    QString login = m_table->item(row, 1)->text();

    QStringList roles = {"user", "admin"};
    bool ok;
    QString role = QInputDialog::getItem(this, "Роль", "Новая роль для " + login + ":",
                                         roles, 0, false, &ok);
    if (!ok) return;

    m_pendingAction = "set_role";
    NetworkManager::getInstance()->sendRequest({
        {"action","set_role"},{"login",login},{"role",role}
    });
}

void AdminPanel::onResponse(const QJsonObject &resp) {
    if (m_pendingAction == "get_users") {
        m_pendingAction.clear();
        if (resp["status"] == "ok")
            m_table->showUsers(resp["users"].toArray());
        else
            m_status->setText("Ошибка: " + resp["message"].toString());
    }
    else if (m_pendingAction == "delete_user" || m_pendingAction == "set_role") {
        m_pendingAction.clear();
        bool ok = resp["status"] == "ok";
        m_status->setStyleSheet(ok ? "color: green;" : "color: red;");
        m_status->setText(resp["message"].toString());
        if (ok) refresh();
    }
}
