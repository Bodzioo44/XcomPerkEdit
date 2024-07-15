#include "MainWindow.h"
#include <QtWidgets/QApplication>

#include "utils.h"
#include "json11/json11.hpp"

#include <iostream>
#include <string>
#include <map>
using namespace std;
using namespace json11;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}