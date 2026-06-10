#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("Var6");
    app.setOrganizationName("ITMO");
    app.setStyle("Fusion");

    MainWindow w;
    w.show();
    return app.exec();
}
