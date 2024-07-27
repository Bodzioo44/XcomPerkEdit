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

    std::vector<QHBoxLayout*> rows = { ui.Row1HBoxLayout, ui.Row2HBoxLayout, ui.Row3HBoxLayout, ui.Row4HBoxLayout, ui.Row5HBoxLayout, ui.Row6HBoxLayout };
    for (int i = 0; i < 18; i++) {
        PerkButton* button = new PerkButton(this);
        rows[i / 3]->addWidget(button);
        perk_buttons.push_back(button);
        connect(button, &QToolButton::clicked, this, [this, i] { this->onPerkSelected(i); });
    }

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
                    //vector<string> split = splitString(hdr.save_description.str, '-');
                    std::vector<std::string> split;
                    std::stringstream ss(hdr.save_description.str);
                    std::string value;
                    while (std::getline(ss, value, '-')) {
                        split.push_back(value);
                    }
                    std::string desc = split[0].substr(0, split[0].size()-1) + " - " + split[1].substr(1) + " \n" + split[2].substr(1) + "\n" + split[3].substr(1, split[3].size()-2) + " - " + split[4].substr(1);
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
    ui.SoldierListWidget->clear();
    //temp backup
    QDir dir("../backup");
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString dateTimeNow = QDateTime::currentDateTime().toString("yy-MM-dd_hh-mm-ss");
    QString sourcePath = ui.PathLineEdit->text() + QString::fromStdString(save_translation[ui.SaveListWidget->currentRow()]);
    QString destPath = QString("../backup/") + QString::fromStdString(save_translation[ui.SaveListWidget->currentRow()] + "_original_"+ dateTimeNow.toStdString());
    QFile::copy(sourcePath, destPath);

    std::string path = ui.PathLineEdit->text().toStdString() + save_translation[ui.SaveListWidget->currentRow()];
    save = xcom::read_xcom_save(path);
    int i = 0;
    xcom::checkpoint_chunk_table& checkpoint_chunk_table = save.checkpoints;
    xcom::checkpoint_chunk& checkpoint_chunk = checkpoint_chunk_table[0];
    xcom::checkpoint_table& checkpoint_table = checkpoint_chunk.checkpoints;
    checkpoint_table_ptr = &checkpoint_table;
    for (const xcom::checkpoint& soldier_checkpoint : checkpoint_table) {
        //Check if entry is a soldier
        if (soldier_checkpoint.name.find("XGStrategySoldier") != std::string::npos) {
            //Various checks if soldier is valid for the editor.
            const xcom::property_list* properties = &soldier_checkpoint.properties;
            if (GetSoldiers::eStatus(properties) != "eStatus_Dead" && GetSoldiers::rank(properties) > 1 && GetSoldiers::class_type(properties) != "") {
                std::string full_name = GetSoldiers::full_name(properties);
                std::string icon_path = "../assets/icons/" + GetSoldiers::class_type(properties) + "_icon.png";

                QIcon icon (QString::fromStdString(icon_path));
                QListWidgetItem* item = new QListWidgetItem(icon, QString::fromStdString(full_name));
                ui.SoldierListWidget->addItem(item);

                index_translation[ui.SoldierListWidget->count() - 1] = i;
            }
        }
        i++;
    }

    //TODO: add a check if no soldiers were found? (very unlikely)
    ui.stackedWidget->setCurrentWidget(ui.PerkEditPage);
    onSoldierSelected();
}

void MainWindow::onSoldierSelected() {
    int current_row = ui.SoldierListWidget->currentRow(); //current row on the list
    int soldier_index = index_translation[current_row]; //soldier index in the save file

    if (soldiers_to_save.find(soldier_index) == soldiers_to_save.end()) {
        soldiers_to_save[soldier_index] = Soldier(&checkpoint_table_ptr->at(soldier_index));
        //soldiers_to_save.emplace(soldier_index, &checkpoint_table_ptr->at(soldier_index));
        //soldiers_to_save.insert_or_assign(soldier_index, &checkpoint_table_ptr->at(soldier_index));
    }
    current_soldier = &soldiers_to_save[soldier_index];
    int soldier_rank = GetSoldiers::rank(current_soldier->GetPropertyList());

    LabelSet labels = current_soldier->GetLabels();
    ui.MobilityLabel->setText(QString::fromStdString(labels[0]));
    ui.AimLabel->setText(QString::fromStdString(labels[1]));
    ui.WillLabel->setText(QString::fromStdString(labels[2]));


    PerkSet soldier_perks = current_soldier->GetPerks();
    PerkDisplayMap perk_display_map = load_perk_display(soldier_perks);

    //grey out all other perks in the same row
    //TODO: do this check whenever loading soldiers?
    for (int i = 0; i < 18; i++) {
        const Perk& current_perk = soldier_perks[i];
        perk_buttons[i]->LoadPerk(perk_display_map[current_perk.index]);
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
        // if ((i+1) % 3 == 0 && !soldier_perks[i].enabled && !soldier_perks[i-1].enabled && !soldier_perks[i-2].enabled) {
        //     perk_buttons[i]->setDisabled(true);
        //     perk_buttons[i-1]->setDisabled(true);
        //     perk_buttons[i-2]->setDisabled(true);
        //     //cout << "disabling: " << i << " " << i-1 << " " << i-2 << endl;
        // }
    }
}

void MainWindow::onPerkSelected(int i) {
    int start_index = (i / 3) * 3;
    for (int j = start_index; j < start_index + 3; j++) {
        if (j == i) {
            continue;
        }
        current_soldier->DisablePerk(j);
        perk_buttons[j]->GreyOut();
    }
    current_soldier->EnablePerk(i);
    perk_buttons[i]->LightUp();

    LabelSet labels = current_soldier->GetLabels();
    ui.MobilityLabel->setText(QString::fromStdString(labels[0]));
    ui.AimLabel->setText(QString::fromStdString(labels[1]));
    ui.WillLabel->setText(QString::fromStdString(labels[2]));
}

void MainWindow::SaveButtonClicked() {
    for (auto& pair : soldiers_to_save) {
        pair.second.UpdateSoldier();
    }

    std::string path = ui.PathLineEdit->text().toStdString() + save_translation[ui.SaveListWidget->currentRow()];
    xcom::write_xcom_save(save, path);
    soldiers_to_save.clear();
    ui.stackedWidget->setCurrentWidget(ui.SavePageWidget);
}

void MainWindow::ExitButtonClicked() {
    ui.stackedWidget->setCurrentWidget(ui.SavePageWidget);
}