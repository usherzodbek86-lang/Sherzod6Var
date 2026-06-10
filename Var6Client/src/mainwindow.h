#pragma once
#include <QMainWindow>
#include <QLabel>
#include <QStackedWidget>

class FunctionPanel;
class AdminPanel;

/**
 * @brief Главное окно приложения.
 *
 * После успешного входа отображает FunctionPanel.
 * Для администратора дополнительно показывает AdminPanel.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onLoginSuccess(const QString &login, const QString &role);
    void onConnectionLost();

private:
    void setupUi();
    void showMainContent(const QString &login, const QString &role);

    QLabel         *m_userLabel;
    QStackedWidget *m_stack;
    FunctionPanel  *m_funcPanel  = nullptr;
    AdminPanel     *m_adminPanel = nullptr;
};
