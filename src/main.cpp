#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtGui/QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/assets/icons/app-icon.png"));
    MainWindow w;
    w.show();
    return a.exec();
}
