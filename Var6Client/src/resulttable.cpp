#include "resulttable.h"
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonObject>

ResultTable::ResultTable(QWidget *parent) : QTableWidget(parent) {
    setColumnCount(2);
    setHorizontalHeaderLabels({"Параметр", "Значение"});
    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    verticalHeader()->setVisible(false);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setAlternatingRowColors(true);
    setupStyle();
}

void ResultTable::setupStyle() {
    setStyleSheet(R"(
        QTableWidget {
            border: 1px solid #bdc3c7;
            gridline-color: #ecf0f1;
            font-size: 13px;
        }
        QHeaderView::section {
            background-color: #2c3e50;
            color: white;
            padding: 6px;
            font-weight: bold;
            border: none;
        }
        QTableWidget::item:alternate {
            background-color: #f5f6fa;
        }
        QTableWidget::item:selected {
            background-color: #3498db;
            color: white;
        }
    )");
}

void ResultTable::showResult(const QList<QPair<QString,QString>> &rows) {
    clearAll();
    setColumnCount(2);
    setHorizontalHeaderLabels({"Параметр", "Значение"});

    setRowCount(rows.size());
    for (int i = 0; i < rows.size(); ++i) {
        auto *keyItem = new QTableWidgetItem(rows[i].first);
        keyItem->setFont(QFont("", -1, QFont::Bold));
        setItem(i, 0, keyItem);

        auto *valItem = new QTableWidgetItem(rows[i].second);
        valItem->setToolTip(rows[i].second); // полный текст во всплывающей подсказке
        setItem(i, 1, valItem);
    }
    resizeRowsToContents();
}

void ResultTable::showUsers(const QJsonArray &users) {
    clearAll();
    setColumnCount(3);
    setHorizontalHeaderLabels({"ID", "Логин", "Роль"});
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    setRowCount(users.size());
    for (int i = 0; i < users.size(); ++i) {
        QJsonObject u = users[i].toObject();
        setItem(i, 0, new QTableWidgetItem(QString::number(u["id"].toInt())));
        setItem(i, 1, new QTableWidgetItem(u["login"].toString()));
        setItem(i, 2, new QTableWidgetItem(u["role"].toString()));
    }
}

void ResultTable::clearAll() {
    setRowCount(0);
}
