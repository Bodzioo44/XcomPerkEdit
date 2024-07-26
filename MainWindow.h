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

#include <vector>
#include <map>
#include <string>

#include "xcomsave/xcom.h"
#include "build/ui_QtDesigner_v2.h"
#include "Qt/PerkButton.h"
#include "Soldier.h"

//xcomsave headers for loading the savefile header
//#include "xcomsave/xcomio.h"



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
    xcom::saved_game save;
    xcom::checkpoint_table* checkpoint_table_ptr;
    std::map<int, int> index_translation;
    std::map<int, std::string> save_translation;
    std::vector<PerkButton*> perk_buttons;
    Soldier* current_soldier;
    //TODO: best way to store the soldiers to save?
    std::map<int, Soldier> soldiers_to_save;
    QIcon save_icon;
    QFont bold_font;
};



#endif