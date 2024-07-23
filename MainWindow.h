#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtGui/QIcon>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#include <QtCore/QDateTime>

#include <map>
#include <vector>
#include <iostream>

#include "build/ui_QtDesigner_v2.h"
#include "Qt/PerkButton.h"
#include "json11.hpp"
#include "utils.h"

//xcomsave headers for loading the savefile header
#include "xcomsave/xcomio.h"
#include "xcomsave/xcom.h"

using namespace json11;
using namespace std;

class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow() {};

public slots:
    //Load the soldier's perks (whenever soldier from the list is selected)
    void onSoldierSelected();
    //Load the save (whenever a save from the list is selected)
    void onSaveSelected();
    //Edit the soldier's perks (whenever PerkButton is clicked)
    void onPerkSelected(int i);
    //Update the savefile
    void SaveButtonClicked();
    //Update the path
    void SelectPathButtonClicked();
    //Exit Button
    void ExitButtonClicked();



private:
    Ui::MainWindow ui;
    Json json;
    map<int, int> index_translation;
    map<int, string> save_translation;
    vector<PerkButton*> perk_buttons;
    Soldier current_soldier;
    //map is the best way to avoid duplicates for the same soldier?
    map<int, Soldier> soldiers_to_save;
    QIcon save_icon;
    QFont bold_font;
};



#endif