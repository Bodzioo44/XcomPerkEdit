#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {
    ui.setupUi(this);
    this->setCentralWidget(ui.centralwidget);
    ui.centralwidget->setLayout(ui.gridLayout);
    ui.PerkEditPage->setLayout(ui.horizontalLayout);
    ui.SavePageWidget->setLayout(ui.SavePageVBoxLayout);
    ui.stackedWidget->setCurrentWidget(ui.SavePageWidget);

    connect(ui.SoldierListWidget, &QListWidget::itemSelectionChanged, this, &MainWindow::onSoldierSelected);
    connect(ui.SaveListWidget, &QListWidget::itemSelectionChanged, this, &MainWindow::onSaveSelected);
    connect(ui.SaveFileButton, &QPushButton::clicked, this, &MainWindow::SaveButtonClicked);
    connect(ui.SelectPathButton, &QPushButton::clicked, this, &MainWindow::SelectPathButtonClicked);
    connect(ui.ExitButton, &QPushButton::clicked, this, &MainWindow::ExitButtonClicked);

    vector<QHBoxLayout*> rows = { ui.Row1HBoxLayout, ui.Row2HBoxLayout, ui.Row3HBoxLayout, ui.Row4HBoxLayout, ui.Row5HBoxLayout, ui.Row6HBoxLayout };
    for (int i = 0; i < 18; i++) {
        PerkButton* button = new PerkButton(this);
        rows[i / 3]->addWidget(button);
        perk_buttons.push_back(button);
        connect(button, &QToolButton::clicked, this, [this, i] { this->onPerkSelected(i); });
    }
    //FIXME: why is this here?
    current_soldier.json_index = -1;

    //TODO: try to move this inside .ui file
    ui.SaveListWidget->setStyleSheet("QListWidget::item { padding: 5px; }");
    ui.SaveListWidget->setIconSize(QSize(100, 50));
    save_icon = QIcon("../assets/icons/appswitcher-xcom-ew-active.png");
    bold_font.setBold(true);

    //TODO: add a check for the OS and set the path accordingly
    QString home_path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString windows_path = "/Documents/My Games/XCOM - Enemy Within/XComGame/SaveData/";
    QString linux_path = "/.local/share/feral-interactive/XCOM/XEW/savedata/";
    QString path = home_path + linux_path;
    ui.PathLineEdit->setText(path);
}

void MainWindow::SelectPathButtonClicked() {
    ui.SaveListWidget->clear();
    QString path = ui.PathLineEdit->text();
    QDir dir(path);
    dir.setFilter(QDir::Files);
    QStringList file_names = dir.entryList(QDir::NoFilter, QDir::Time);
    for (QString& name : file_names) {
        if (name.contains("save")) {
            //TODO: remove try/catch (not needed anymore since now only header is being loaded?)
            //TODO: keep it as a check if file is a xcom save?
            try {
                QString file_path = path + name;
                xcom::header hdr = xcom::read_only_header(file_path.toStdString());
                if (!hdr.tactical_save) {
                    //formats hdr.save_description.str into a more readable format
                    vector<string> split = splitString(hdr.save_description.str, '-');
                    string desc = split[0].substr(0, split[0].size()-1) + " - " + split[1].substr(1) + " \n" + split[2].substr(1) + "\n" + split[3].substr(1, split[3].size()-2) + " - " + split[4].substr(1);
                    QListWidgetItem* item = new QListWidgetItem(save_icon, QString::fromStdString(desc));
                    item->setFont(bold_font);
                    item->setToolTip(name);
                    ui.SaveListWidget->addItem(item);
                    save_translation[ui.SaveListWidget->count() -1] = name.toStdString();
                }
            }
            catch (xcom::error::general_exception& e) {
                //expected error while trying to load save that is non tactical save
                //TODO: try to load just the header so this check is not needed.
            }
        }
    }
    //TODO: add some popup window whenever no saves are found?
}

void MainWindow::onSaveSelected() {
    QDir dir("../backup");
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString dateTimeNow = QDateTime::currentDateTime().toString("yy-MM-dd_hh-mm-ss");
    QString sourcePath = ui.PathLineEdit->text() + QString::fromStdString(save_translation[ui.SaveListWidget->currentRow()]);
    QString destPath = QString("../backup/") + QString::fromStdString(save_translation[ui.SaveListWidget->currentRow()] + "_original_"+ dateTimeNow.toStdString());
    QFile::copy(sourcePath, destPath);

    ui.SoldierListWidget->clear();
    //TODO: remove try/catch? (not needed anymore since the header check?)
    try {
        string path = ui.PathLineEdit->text().toStdString() + save_translation[ui.SaveListWidget->currentRow()];
        json = load_json_file(path);
        int i = 0;
        for (const Json& entry : json["checkpoints"][0]["checkpoint_table"].array_items()) {
            //Checks if entry is a soldier
            if (entry["name"].string_value().find("XGStrategySoldier") != string::npos) {
                //Checks if soldier is alive, has class assigned and has rank above specialist
                if (Get_Soldiers::eStatus(json, i) != "eStatus_Dead" && Get_Soldiers::rank(json, i) > 1 && Get_Soldiers::class_type(json, i) != "") {
                    string soldier_fullname = Get_Soldiers::firstname(json, i) + " '" + Get_Soldiers::nickname(json, i) + "' " + Get_Soldiers::lastname(json, i);
                    string icon_path = "../assets/icons/" + Get_Soldiers::class_type(json, i) + "_icon" + ".png";

                    QIcon icon(QString::fromStdString(icon_path));
                    QListWidgetItem* item = new QListWidgetItem(icon, QString::fromStdString(soldier_fullname));
                    ui.SoldierListWidget->addItem(item);

                    index_translation[ui.SoldierListWidget->count() - 1] = i; 
                    //debugging
                    if (Get_Soldiers::class_type(json, i) == "Engineer") {
                        cout << "Engineer found: " << Get_Soldiers::nickname(json, i) << endl;
                        cout << "Perk index 22 value: " <<  Get_Soldiers::upgrades(json, i)[44] << endl << endl;

                    }
                }
            }
            i++;
        }
    }
    catch (runtime_error& e) {
        cerr << e.what() << endl;
    }
    //TODO: add a check if no soldiers were found? (very unlikely)
    ui.stackedWidget->setCurrentWidget(ui.PerkEditPage);
    onSoldierSelected();
}

void MainWindow::onSoldierSelected() {
    int current_row = ui.SoldierListWidget->currentRow(); //current row on the list
    int soldier_index = index_translation[current_row]; //soldier index in the save file
    int soldier_rank = Get_Soldiers::rank(json, soldier_index);
    SoldierStats stats = Get_Soldiers::stats(json, soldier_index);

    ui.MobilityLabel->setText(QString::fromStdString("<b>Mobility: " + to_string(stats.mobility) + "</b>"));
    ui.AimLabel->setText(QString::fromStdString("<b>Aim: " + to_string(stats.aim) + "</b>"));
    ui.WillLabel->setText(QString::fromStdString("<b>Will: " + to_string(stats.will) + "</b>"));

    vector<Perk> soldier_perks = load_perks(json, soldier_index); //vector of soldier perks (in order)
    perk_map perk_info = load_perk_info(soldier_perks); //map of perk index to perk data

    if (current_soldier.json_index != -1) { 
        soldiers_to_save[current_soldier.json_index] = current_soldier;
    }

    //grey out all other perks in the same row
    //TODO: do this check whenever loading soldiers?
    for (int i = 0; i < 18; i++) {
        const Perk& current_perk = soldier_perks[i];
        perk_buttons[i]->LoadPerk(perk_info[current_perk.index], current_perk);
        if (current_perk.enabled) {
            perk_buttons[i]->LightUp();
        }
        else {
            perk_buttons[i]->GreyOut();
        }

        //disable perks that are not available yet (based on soldier rank)
        if ((i+1) > (soldier_rank-1)*3) {
            perk_buttons[i]->setDisabled(true);
        }
        else {
            perk_buttons[i]->setDisabled(false);
            //cout << "enabling: " << i << endl;
        }
        //disable perks from rank that wasnt assigned yet.
        //assigning perk for the first time should be done in game, otherwise it messes with stats increase on level up.
        //TODO: move this to onSaveSelected soldier availability check?
        if ((i+1) % 3 == 0 && !soldier_perks[i].enabled && !soldier_perks[i-1].enabled && !soldier_perks[i-2].enabled) {
            perk_buttons[i]->setDisabled(true);
            perk_buttons[i-1]->setDisabled(true);
            perk_buttons[i-2]->setDisabled(true);
            //cout << "disabling: " << i << " " << i-1 << " " << i-2 << endl;
        }
    }
    current_soldier.current_stats = stats;
    current_soldier.stats_diff = { 0, 0, 0 };
    current_soldier.perks = soldier_perks;
    current_soldier.json_index = soldier_index;
}

void MainWindow::onPerkSelected(int i) {
    int start_index = (i / 3) * 3;
    for (int j = start_index; j < start_index + 3; j++) {
        if (j == i) {
            continue;
        }
        current_soldier.Disable_Perk(j);
        perk_buttons[j]->GreyOut();
    }
    current_soldier.Enable_Perk(i);
    perk_buttons[i]->LightUp();

    array<string, 3> stats_labels = current_soldier.Calculate_Stats();
    ui.MobilityLabel->setText(QString::fromStdString(stats_labels[0]));
    ui.AimLabel->setText(QString::fromStdString(stats_labels[1]));
    ui.WillLabel->setText(QString::fromStdString(stats_labels[2]));
}

void MainWindow::SaveButtonClicked() {
    //iterate over soldiers_to_save map, update the json for each one, and save it.
    if (current_soldier.json_index != -1) {
    soldiers_to_save[current_soldier.json_index] = current_soldier;
    }
    for (const auto& pair : soldiers_to_save) {
        update_json(json, pair.second);
    }
    string path = ui.PathLineEdit->text().toStdString() + save_translation[ui.SaveListWidget->currentRow()];
    save_json_file(path, json);
    soldiers_to_save.clear();
    current_soldier.json_index = -1;
    ui.stackedWidget->setCurrentWidget(ui.SavePageWidget);

    QDir dir("../backup");
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString dateTimeNow = QDateTime::currentDateTime().toString("yy-MM-dd_hh-mm-ss");
    QString sourcePath = ui.PathLineEdit->text() + QString::fromStdString(save_translation[ui.SaveListWidget->currentRow()]);
    QString destPath = QString("../backup/") + QString::fromStdString(save_translation[ui.SaveListWidget->currentRow()] + "_edited_"+ dateTimeNow.toStdString());
    QFile::copy(sourcePath, destPath);
}

void MainWindow::ExitButtonClicked() {
    ui.stackedWidget->setCurrentWidget(ui.SavePageWidget);
}