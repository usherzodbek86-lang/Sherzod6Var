#include "mainwindow.h"
#include "loginwindow.h"
#include "functionpanel.h"
#include "adminpanel.h"
#include "networkmanager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QStatusBar>
#include <QMenuBar>
#include <QToolBar>
#include <QApplication>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Var6 — Система шифрования и стеганографии");
    resize(900, 650);
    setupUi();

    connect(NetworkManager::getInstance(), &NetworkManager::connectionLost,
            this, &MainWindow::onConnectionLost);

    // Показываем окно входа при старте
    LoginWindow *lw = new LoginWindow(this);
    connect(lw, &LoginWindow::loginSuccess, this, &MainWindow::onLoginSuccess);
    lw->exec();
}

void MainWindow::setupUi() {
    // Меню
    auto *fileMenu = menuBar()->addMenu("Файл");
    fileMenu->addAction("Выход", qApp, &QApplication::quit);

    auto *helpMenu = menuBar()->addMenu("Справка");
    helpMenu->addAction("О программе", this, [this]{
        QMessageBox::about(this, "О программе",
            "<b>Var6 — Курсовая работа</b><br>"
            "Ахроров Шерзодбек Туймурод угли<br>"
            "Группа 251-354<br><br>"
            "RSA | SHA-1 | Метод Ньютона | WAV-стеганография");
    });

    // Центральный виджет
    auto *central = new QWidget(this);
    auto *root    = new QVBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // Шапка с информацией о пользователе
    auto *header = new QWidget;
    header->setStyleSheet("background-color: #2c3e50; color: white; padding: 8px;");
    auto *hRow = new QHBoxLayout(header);
    m_userLabel = new QLabel("Не авторизован");
    m_userLabel->setStyleSheet("color: white; font-weight: bold; font-size: 13px;");
    hRow->addWidget(m_userLabel);
    hRow->addStretch();
    root->addWidget(header);

    // Стек страниц
    m_stack = new QStackedWidget;
    auto *placeholder = new QLabel("Войдите в систему");
    placeholder->setAlignment(Qt::AlignCenter);
    placeholder->setStyleSheet("font-size: 18px; color: #95a5a6;");
    m_stack->addWidget(placeholder); // индекс 0
    root->addWidget(m_stack);

    setCentralWidget(central);
    statusBar()->showMessage("Готов к работе");
}

void MainWindow::onLoginSuccess(const QString &login, const QString &role) {
    m_userLabel->setText(QString("Пользователь: %1  |  Роль: %2").arg(login, role));

    // Убираем заглушку, строим рабочий интерфейс
    // (только при первом входе)
    if (!m_funcPanel) {
        auto *tabs = new QTabWidget;
        m_funcPanel = new FunctionPanel;
        tabs->addTab(m_funcPanel, "Функции");

        if (role == "admin") {
            m_adminPanel = new AdminPanel;
            tabs->addTab(m_adminPanel, "👤 Администрирование");
            m_adminPanel->refresh();
        }

        m_stack->addWidget(tabs);  // индекс 1
    }

    m_stack->setCurrentIndex(1);
    statusBar()->showMessage("Подключено | " + login);
}

void MainWindow::onConnectionLost() {
    statusBar()->showMessage("Соединение потеряно!");
    QMessageBox::warning(this, "Соединение",
        "Соединение с сервером разорвано.");
}
