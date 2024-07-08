#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtGui/QIcon>
#include <map>
#include <vector>
#include <iostream>


#include "build/ui_QtDesigner.h"
#include "json11/json11.hpp"
#include "utils.h"


using namespace json11;
using namespace std;

class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow() {};

public slots:
    void onSoldierSelected();



private:
    Ui::MainWindow ui;
    Json json;
    map<int, int> index_translation;

};



#endif