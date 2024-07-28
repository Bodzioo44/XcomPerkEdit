#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtGui/QIcon>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtWidgets/QProgressDialog>
#include <QtCore/QSettings>
#include <QtCore/QTimer>

#include <vector>
#include <map>
#include <string>
#include <chrono>

#include "xcomsave/xcom.h"
#include "build/ui_QtDesigner.h"
#include "Qt/PerkButton.h"
#include "Soldier.h"

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
    std::map<int, int> soldier_index_translation;
    //TODO: Get rid of the current_dir, and just store full path to the file in the save_index_translation?
    //std::map<int, QString> save_index_translation;
    //now its being stored in ListWidgetItem tooltip, which is probably bad.  
    QDir current_dir;
    std::vector<PerkButton*> perk_buttons;
    Soldier* current_soldier;
    //TODO: best way to store the soldiers to save?
    //switch to vector? map is problematic...
    std::map<int, Soldier> soldiers_to_save;
    QIcon save_icon;
    QFont bold_font;

    void GenerateINIFile();
    void LoadINIFile();
    int backup_limit;
    QString save_dir_path;
};



#endif