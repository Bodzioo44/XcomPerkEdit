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
#include <QtWidgets/QScrollBar>
#include <QtGui/QPainter>

#include <vector>
#include <map>
#include <string>
#include <chrono>

#include "xcomsave/xcom.h"
#include "ui_QtDesigner.h"
#include "Qt/CustomQt.h"
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
    //Revert Soldier Changes
    void RevertSoldierClicked();
    //Revert All Changes
    void RevertAllClicked();
    //Apply Appearance Preset
    void ApplyAppearancePreset();
    //Move steam vent
    void VentButtonClicked();
private:
    Ui::MainWindow ui;
    xcom::saved_game save;
    xcom::checkpoint_table* checkpoint_table_ptr;
    xcom::number_array_property* number_array_SteamTiles_ptr;
    xcom::struct_array_property* struct_array_Tiles_ptr;

    std::map<QTreeWidgetItem*, int> soldier_index_translation;
    //TODO: Get rid of the current_dir, and just store full path to the file in the save_index_translation?
    //std::map<int, QString> save_index_translation;
    //now its being stored in ListWidgetItem tooltip, which is probably bad.  
    QDir current_dir;
    QSettings settings;
    std::vector<PerkButton*> perk_buttons;
    Soldier* current_soldier;
    //TODO: best way to store the soldiers to save?
    //switch to vector? map is problematic...
    std::map<int, Soldier> soldiers_to_save;

    void GenerateINIFile();
    void LoadINIFile();
    int32_t int_property_val(xcom::int_property* prop);

    int backup_limit;
    QString save_dir_path;
    bool auto_load_last_path;
    bool stay_after_save;

    const std::map<int,QString> rank_translation =
    {
        {2, ":/assets/icons/RANK_CORPORAL.png"},
        {3, ":/assets/icons/RANK_SERGEANT.png"},
        {4, ":/assets/icons/RANK_LIEUTENANT.png"},
        {5, ":/assets/icons/RANK_CAPTAIN.png"},
        {6, ":/assets/icons/RANK_MAJOR.png"},
        {7, ":/assets/icons/RANK_COLONEL.png"},
    };
};

#endif
