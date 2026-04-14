#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), settings("config.ini", QSettings::IniFormat) {
    ui.setupUi(this);
    this->setCentralWidget(ui.centralwidget);
    ui.centralwidget->setLayout(ui.gridLayout);
    ui.PerkEditPage->setLayout(ui.horizontalLayout);
    ui.SavePageWidget->setLayout(ui.SavePageVBoxLayout);
    ui.stackedWidget->setCurrentWidget(ui.SavePageWidget);

    connect(ui.SoldierTreeWidget, &QTreeWidget::currentItemChanged, this, &MainWindow::onSoldierSelected);
    connect(ui.SaveListWidget, &QListWidget::itemActivated, this, &MainWindow::onSaveSelected);
    connect(ui.SaveFileButton, &QPushButton::clicked, this, &MainWindow::SaveButtonClicked);
    connect(ui.SelectPathButton, &QPushButton::clicked, this, &MainWindow::SelectPathButtonClicked);
    connect(ui.ExitButton, &QPushButton::clicked, this, &MainWindow::ExitButtonClicked);
    connect(ui.RevertSoldierButton, &QPushButton::clicked, this, &MainWindow::RevertSoldierClicked);
    connect(ui.RevertAllButton, &QPushButton::clicked, this, &MainWindow::RevertAllClicked);
    // connect(ui.AppearanceButton, &QPushButton::clicked, this, &MainWindow::ApplyAppearancePreset);
    connect(ui.VentButton, &QPushButton::clicked, this, &MainWindow::VentButtonClicked);
    connect(ui.PathLineEdit, &QLineEdit::returnPressed, this, &MainWindow::SelectPathButtonClicked);

    std::vector<QHBoxLayout*> rows = { ui.Row1HBoxLayout, ui.Row2HBoxLayout, ui.Row3HBoxLayout, ui.Row4HBoxLayout, ui.Row5HBoxLayout, ui.Row6HBoxLayout };
    for (int i = 0; i < 18; i++) {
        PerkButton* button = new PerkButton(this);
        rows[i / 3]->addWidget(button);
        perk_buttons.push_back(button);
        connect(button, &QToolButton::clicked, this, [this, i] { this->onPerkSelected(i); });
    }

    QTreeWidgetItem* header = new QTreeWidgetItem();
    header->setText(0, "Soldiers Name");
    header->setToolTip(0, "Sort by Name");

    header->setText(1, "Class");
    header->setToolTip(1, "Sort by Class");
    // header->setIcon(1, QIcon(":/assets/icons/RANK_ROOKIE.png"));

    header->setText(2, "Rank");
    header->setToolTip(2, "Sort by Rank");
    // header->setIcon(2, QIcon(":/assets/icons/RANK_SQUADDIE.png"));
    
    ui.SoldierTreeWidget->setHeaderItem(header);
    ui.SoldierTreeWidget->setIndentation(0);
    ui.SoldierTreeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    if (!QFile("config.ini").exists()) {
        GenerateINIFile();
    }
    LoadINIFile();
    ui.PathLineEdit->setText(save_dir_path);
    if (auto_load_last_path) {
        QTimer::singleShot(0, this, [this] {
        SelectPathButtonClicked(); });
    }
}

void MainWindow::SelectPathButtonClicked() {
    // qDebug() << "SelectPathButtonClicked";
    auto start = std::chrono::high_resolution_clock::now();
    ui.SaveListWidget->clear();
    //Check if path exists
    QString path = ui.PathLineEdit->text();
    if (!QDir(path).exists()) {
        QMessageBox::warning(this, "Invalid Path", "Path does not exist.");
        return;
    }
    qDebug() << "Selected path: " << path;
    current_dir = QDir(path);
    QStringList file_names = current_dir.entryList(QDir::Files, QDir::Time);

    //Update the last used path in the config file
    QSettings settings("config.ini", QSettings::IniFormat);
    if (settings.value("SAVE_DIR_PATH").toString() != ui.PathLineEdit->text()) {
        settings.setValue("SAVE_DIR_PATH", ui.PathLineEdit->text());
        qDebug() << "SAVE_DIR_PATH updated: " << ui.PathLineEdit->text();
        settings.sync();
    }
    //Create a progress bar if needed
    QProgressDialog* progress = nullptr;
    if (file_names.size() > 5) {
        progress = new QProgressDialog("Processing saves...", "Abort", 0, file_names.size(), this);
        progress->setWindowModality(Qt::WindowModal);
        progress->show();
    }

    //font and icon for the ui.SaveListWidget items
    QFont bold_font;
    bold_font.setBold(true);
    QIcon save_icon(":/assets/icons/appswitcher-xcom-ew-active.png");
    //Go over the files and load all GEOSCAPE saves
    for (QString& name : file_names) {
        if (name.contains("save")) {
            try {
                QString file_path = current_dir.filePath(name);
                xcom::header hdr = xcom::read_only_header(file_path.toStdString());
                if (!hdr.tactical_save) {
                    //formats hdr.save_description.str into a more readable format
                    std::vector<std::string> split;
                    std::stringstream ss(hdr.save_description.str);
                    std::string value;
                    while (std::getline(ss, value, '-')) {
                        split.push_back(value);
                    }
                    std::string desc = split[0].substr(0, split[0].size()-1) + " - " + split[1].substr(1) + " \n" + split[2].substr(1) + "\n" + split[3].substr(1, split[3].size()-2) + " - " + split[4].substr(1);
                    //create a new item with the save icon and the formatted description.
                    QListWidgetItem* item = new QListWidgetItem(save_icon, QString::fromStdString(desc));
                    item->setFont(bold_font);
                    item->setToolTip(name);
                    ui.SaveListWidget->addItem(item);
                }
            }
            catch (xcom::error::xcom_exception& e) {
                qDebug() << "XCOM exception occured while reading" << name << "header:" << QString::fromStdString(e.what());
            }
        }
        if (progress) {
            progress->setValue(progress->value() + 1);
            QCoreApplication::processEvents();
            if (progress->wasCanceled()) {
                break;
            }
        }
    }
    if (progress) {
        progress->close();
        delete progress;
    }

    auto end = std::chrono::high_resolution_clock::now();
    qDebug() << "Time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms";
    if (ui.SaveListWidget->count() == 0) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("No GEOSCAPE Saves Found");
        msgBox.setTextFormat(Qt::RichText);
        msgBox.setText("No GEOSCAPE xcom saves were found in the selected path, only files containing 'save' in the name are considered.\n");
        #ifdef Q_OS_LINUX
        msgBox.setText(msgBox.text() + 
        "If you are on linux there is a bug that creates 2 separate folders for XCOM and steamcloud.\n"
        "Its caused by case sensitivity of the file system, read more: <a href=\"https://steamcommunity.com/app/200510/discussions/0/613956964581020366/\">here</a>");
        #endif
        msgBox.exec();
    }
}

void MainWindow::onSaveSelected() {
    qDebug() << "Save selected.";
    //clear the list for new soliders.
    //this triggers itemSelectionChanged signal before the clear() method is called on the QListWidget.
    ui.SoldierTreeWidget->clear();

    //backup folder needs to be outside SaveData since xcom also checks subfolders for saves.
    QDir backup_dir(QDir(QCoreApplication::applicationDirPath()).filePath("backup"));
    if (!backup_dir.exists()) {
        qDebug() << "Creating backup directory: " << backup_dir.path();
        backup_dir.mkpath(".");
    }
    QString dateTimeNow = QDateTime::currentDateTime().toString("_dd-MM-yy_hh-mm-ss");
    QString sourcePath = current_dir.filePath(ui.SaveListWidget->currentItem()->toolTip());
    QString destPath = backup_dir.filePath(ui.SaveListWidget->currentItem()->toolTip() + dateTimeNow);
    QFile::copy(sourcePath, destPath);
    qDebug() << "Backup created.";

    //removing old backups
    QFileInfoList backupFiles = backup_dir.entryInfoList(QDir::Files, QDir::Time);
    if (backupFiles.size() > backup_limit) {
        qDebug() << "Deleting old backups...";
        for (int i = backup_limit; i < backupFiles.size(); i++) {
            //backupFiles[i].dir().remove(backupFiles[i].fileName());
            QFile::remove(backupFiles[i].absoluteFilePath());
            qDebug() << "Deleting: " << backupFiles[i].absoluteFilePath();
        }
        qDebug() << "Old backups deleted.";
    }

    std::string path = current_dir.filePath(ui.SaveListWidget->currentItem()->toolTip()).toStdString();
    qDebug() << "Loading save:" << QString::fromStdString( path);
    save = xcom::read_xcom_save(path);
    qDebug() << "Save successfully loaded";
    int i = 0;
    xcom::checkpoint_chunk_table& checkpoint_chunk_table = save.checkpoints;
    xcom::checkpoint_chunk& checkpoint_chunk = checkpoint_chunk_table[0];
    xcom::checkpoint_table& checkpoint_table = checkpoint_chunk.checkpoints;
    checkpoint_table_ptr = &checkpoint_table;
    for (const xcom::checkpoint& soldier_checkpoint : checkpoint_table) {

        if (soldier_checkpoint.name.find("Command1.TheWorld:PersistentLevel.XGBase_") != std::string::npos) {
            qDebug() << "Found XGBase checkpoint";

            // xcomsave failed to detect specific kind of this array and applied generic array_property.
            // we need to manually access raw data and treat it like int32_t array to get the vent values.
            // so in this case every four bytes are a single int32_t, but we are using only every 4th byte since values are lesser than 255.
            // failed or not??? kind_string() returns array_property, but casting it into number_array_property works fine?

            // Saving pointer to steam tile array for later use.
            number_array_SteamTiles_ptr = &static_cast<xcom::number_array_property&> (*soldier_checkpoint.properties.at(2));

            // Saving pointer to tile array for later use.
            struct_array_Tiles_ptr = &static_cast<xcom::struct_array_property&> (*soldier_checkpoint.properties.at(1));

            // Updating QSpinBoxes with current vents values.
            int vent1 = number_array_SteamTiles_ptr->elements.at(0);
            int vent2 = number_array_SteamTiles_ptr->elements.at(1);
            ui.Vent1Box->setValue(vent1);
            ui.Vent2Box->setValue(vent2);

            qDebug() << "Steam vent tile checkpoint found, vent values: " << vent1 << ", " << vent2;

        }
        //Check if entry is a soldier
        if (soldier_checkpoint.name.find("XGStrategySoldier") != std::string::npos) {
            //Various checks if soldier is valid for the editor.
            const xcom::property_list* properties = &soldier_checkpoint.properties;
            if (GetSoldiers::eStatus(properties) != "eStatus_Dead" && GetSoldiers::rank(properties) > 1 && GetSoldiers::class_type(properties) != "") {
                //extra check if soldier has at least 1 perk assigned.
                PerkSet temp_perks = GetSoldiers::perks(properties);
                if (!(temp_perks[0].enabled || temp_perks[1].enabled || temp_perks[2].enabled)) {
                    i++;
                    continue;
                }

                QString full_name = QString::fromStdString(GetSoldiers::full_name(properties));
                QString icon_path = QString::fromStdString(":/assets/icons/" + GetSoldiers::class_type(properties) + "_icon.png");

                QTreeWidgetItem* item = new SoldierTreeItem();

                // Check if soldier can be promoted in game.
                bool promotion_icon = false;
                for (int rank_check = 1; rank_check < GetSoldiers::rank(properties) - 1; rank_check++) {
                    if (!(temp_perks[rank_check * 3].enabled || temp_perks[rank_check * 3 + 1].enabled || temp_perks[rank_check * 3 + 2].enabled)) {
                        promotion_icon = true;
                        break;
                    }
                }
                // QTreeWidgetItem only supports icons in front, so custom widget is needed.
                CustomWidget* widget;
                if (promotion_icon) {
                    widget = new CustomWidget(full_name, QIcon(":/assets/icons/promotion_icon_transparent_fixed.png"));
                }
                else {
                    widget = new CustomWidget(full_name);
                }
                item->setData(0, Qt::UserRole, full_name);

                item->setIcon(1, QIcon(icon_path));
                item->setData(1, Qt::UserRole, icon_path);

                item->setIcon(2, QIcon(rank_translation.at(GetSoldiers::rank(properties))));
                item->setData(2, Qt::UserRole, GetSoldiers::rank(properties));

                // item->setData(3, Qt::UserRole, 0);
                ui.SoldierTreeWidget->addTopLevelItem(item);

                //Set the custom widget 
                ui.SoldierTreeWidget->setItemWidget(item, 0, widget);
                soldier_index_translation[item] = i;
            }
        }
        i++;
    }
    // resize soldier tree to fit the content
    int size = 2;
    for (int i = 0; i < ui.SoldierTreeWidget->columnCount(); i++) {
        size += ui.SoldierTreeWidget->columnWidth(i);
    }
    size += ui.SoldierTreeWidget->verticalScrollBar()->sizeHint().width();
    ui.SoldierTreeWidget->setFixedWidth(size);

    if (ui.SoldierTreeWidget->topLevelItemCount() == 0) {
        QMessageBox::warning(this, "No soldiers found", "No acceptable soldiers were found in the save file.");
        return;
    }
    ui.stackedWidget->setCurrentWidget(ui.PerkEditPage);
    qDebug() << "Checkpoint table loaded.";
    //to always trigger onSoldierSelected() when the save is loaded.
    ui.SoldierTreeWidget->setCurrentItem(ui.SoldierTreeWidget->topLevelItem(0));
}

//QListWidget::currentRowChanged will also trigger on QListWidget::clear().
void MainWindow::onSoldierSelected() {
    qDebug() << "Soldier selected.";

    //Whenever ui.SoldierTreeWidget is being cleared (eg. when a new save is loaded) ui.SoldierTreeWidget->currentItemChanged is triggered before the actuall clear,
    //which makes this method run with empty/old ui.SoldierTreeWidget and probably causes some bad stuff.
    //Maybe clear the ui.SoldierTreeWidget on save quit instead of on save load? or think of less hacky solution.
    if (ui.stackedWidget->currentWidget() == ui.SavePageWidget) {
        // qDebug() << "Invalid row index, probably due to QListWidget::clear()";
        qDebug() << "Tried to call onSoldierSelected while on SavePageWidget, probably due to QListWidget::clear() triggering the signal.";
        return;
    }
    QTreeWidgetItem* current_item = ui.SoldierTreeWidget->currentItem();
    int soldier_index = soldier_index_translation[current_item]; //soldier index in the save file
    qDebug() << "Selected soldier with save index: " << soldier_index;

    if (soldiers_to_save.find(soldier_index) == soldiers_to_save.end()) {
        soldiers_to_save[soldier_index] = Soldier(&checkpoint_table_ptr->at(soldier_index));
        //soldiers_to_save.emplace(soldier_index, &checkpoint_table_ptr->at(soldier_index));
        //soldiers_to_save.insert_or_assign(soldier_index, &checkpoint_table_ptr->at(soldier_index));
    }
    current_soldier = &soldiers_to_save[soldier_index];
    int soldier_rank = GetSoldiers::rank(current_soldier->GetPropertyList());
    // qDebug() << "Soldier rank: " << soldier_rank;
    //soldier stats
    ui.StatsLabel->setText(current_soldier->GetLabels());
    //soldier perks
    PerkSet soldier_perks = current_soldier->GetPerks();

    //map of [perk_index] -> PerkDisplay (name, icon, description)
    PerkDisplayMap perk_display_map = load_perk_display(soldier_perks);
    ui.InfoLabel->clear();

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
        if (i / 3 > soldier_rank - 2 ) {
            perk_buttons[i]->setDisabled(true);
        }
        else {
            perk_buttons[i]->setDisabled(false);
        }
        //disable perks from rank that wasnt assigned yet.
        //all available perks should be assigned in game before changing anything, otherwise it will mess with level up stats.
        if (i % 3 == 2 && i / 3 <= soldier_rank - 2 && !(soldier_perks[i].enabled || soldier_perks[i-1].enabled || soldier_perks[i-2].enabled)) {
            perk_buttons[i]->setDisabled(true);
            perk_buttons[i-1]->setDisabled(true);
            perk_buttons[i-2]->setDisabled(true);
            if (ui.InfoLabel->text().isEmpty()) {
                ui.InfoLabel->setText("<b>Promotion available in game!<br>Some perks wont be editable<br>until you assign rank in game.</b>");
            }
        }
    }
    qDebug() << "Soldier successfully loaded!";
    // qDebug() << "Current soldiers to save: " << soldiers_to_save.size();
    // for (auto& pair : soldiers_to_save) {
    //     qDebug() << " " << pair.first;
    // }
}

void MainWindow::onPerkSelected(int i) {
    // qDebug() << "onPerkSelected";
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
    ui.StatsLabel->setText(current_soldier->GetLabels());
}

void MainWindow::SaveButtonClicked() {
    qDebug() << "\nSaving... (Save button clicked)";
    //TODO: replace with a bool that tracks if any soldier was modified.
    //check below is always active since selecting a soldier adds it to the map, even if it was not modified.
    //maybe remove the check entirely?
    if (!soldiers_to_save.empty()) {
        for (auto& pair : soldiers_to_save) {
            pair.second.UpdateSoldier();
        }
        std::string path = current_dir.filePath(ui.SaveListWidget->currentItem()->toolTip()).toStdString();
        xcom::write_xcom_save(save, path);
        qDebug() << "Successfully saved the game.";
        QMessageBox::information(this, "Save successful", "Save successful!");
        current_soldier = nullptr;
        soldiers_to_save.clear();

        if (!stay_after_save) {
            ui.SaveListWidget->clearSelection();
            ui.stackedWidget->setCurrentWidget(ui.SavePageWidget);
        }
        else {
            onSoldierSelected();
        }
    }
    else {
        QMessageBox::warning(this, "No soldiers selected", "No soldiers were selected for editing.");
    }
}

int32_t MainWindow::int_property_val(xcom::int_property* prop) {
    if (prop != nullptr) {
        return prop->value;
    }
    else {
        return 0;
    }
}

void MainWindow::VentButtonClicked() {

    int32_t vent1 = ui.Vent1Box->value();
    int32_t vent2 = ui.Vent2Box->value();

    int32_t x1, y1, x2, y2;
    x1 = vent1 % 7; y1 = vent1 / 7;
    x2 = vent2 % 7; y2 = vent2 / 7;

    qDebug() << "\nNew vent values: (" << x1 << ", " << y1 << "), (" << x2 << ", " << y2 << ") (vent button clicked)";

    // Editing m_arrSteamTiles data to match new vent location, I'm not sure if it actually does anything?
    number_array_SteamTiles_ptr->elements.at(0) = vent1;
    number_array_SteamTiles_ptr->elements.at(1) = vent2;

    // Vent locations are tied to iType of m_arrTiles structs? I'm pretty sure I moved them just by editing m_arrSteamTiles before. Whats the point of m_arrSteamTiles then?
    // iType = 0 is unexcavated normal tile.
    // iType = 1 is unexcavated Steam Vent.
    // iType = 2 is manually excavated tile.
    // iType = 3 is tile that was generated already excavated.
    // iType = 4 is steam vent that was excavated (idk if excavated steam vents can generate, or what iType they would have).
    // iType = 5 is tile that is currently occupied by a structure.
    // How would tile with steam vent occupied by a structure look like?

    // iTileState is 1 for accessible? 2 for inaccessible? (you can start excavating or building something on accesible tiles)

    // Editing iType values to match new vents location
    xcom::int_property* X_prop, *Y_prop, *iTileState_prop, *iType_prop;
    for (xcom::property_list& structs : struct_array_Tiles_ptr->elements) {
        X_prop = nullptr; Y_prop = nullptr; iTileState_prop = nullptr; iType_prop = nullptr;
        for (xcom::property_ptr& prop : structs) {
            xcom::int_property* int_prop = static_cast<xcom::int_property*>(prop.get());
            if (int_prop->name == "X") {
                X_prop = int_prop;
            }
            else if (int_prop->name == "Y") {
                Y_prop = int_prop;
            }
            else if (int_prop->name == "iTileState") {
                iTileState_prop = int_prop;
            }
            else if (int_prop->name == "iType") {
                iType_prop = int_prop;
            }
        }
        if ((int_property_val(X_prop) == x1 && int_property_val(Y_prop) == y1) || (int_property_val(X_prop) == x2 && int_property_val(Y_prop) == y2)) {
            qDebug() << "Checking location of new vent at: (" << int_property_val(X_prop) << ", " << int_property_val(Y_prop) << ")";
            // iType might not exist if its equal to 0. 
            if (iType_prop == nullptr) {
                qDebug() << "iType property not found, placing new unique_ptr<xcom::int_property> at the end of the struct vector.";
                structs.push_back(std::make_unique<xcom::int_property>("iType", 0));
                iType_prop = static_cast<xcom::int_property*>(structs.back().get());
            }
            int32_t val = iType_prop->value;
            if (val == 0) {
                qDebug() << "Replacing unexcavated tile with unexcavated steam vent at: (" << int_property_val(X_prop) << ", " << int_property_val(Y_prop) << ")";
                iType_prop->value = 1;
            }
            else if (val == 2 || val == 3) { 
                qDebug() << "Replacing excavated tile with excavated steam vent at: (" << int_property_val(X_prop) << ", " << int_property_val(Y_prop) << ")";
                iType_prop->value = 4;
            }
            else {
                qDebug() << "Unexpected iType value at vent location, or steam vent is already there: (" << int_property_val(X_prop) << ", " << int_property_val(Y_prop) << "), iType: " << val;
            }
        }
        // Remove old vents.
        // Replace unexcavated steam vent with unexcavated tile.
        else if (int_property_val(iType_prop) == 1) {
            qDebug() << "Replacing unexcavated steam vent with unexcavated tile at: (" << int_property_val(X_prop) << ", " << int_property_val(Y_prop) << ")";
            iType_prop->value = 0;
        }
        // Replace excavated steam vent with excavated.
        else if (int_property_val(iType_prop) == 4) {
            qDebug() << "Replacing excavated steam vent with excavated tile at: (" << int_property_val(X_prop) << ", " << int_property_val(Y_prop) << ")";
            iType_prop->value = 2;
        }
    }
}

// void MainWindow::ApplyAppearancePreset() {
//     // qDebug() << "ApplyAppearancePreset";
//     if (current_soldier) {
//         QVariantList loaded = settings.value("APPEARANCE_PRESET", QVariantList()).toList();
//         AppearanceSet appearance;
//         int i = 0;
//         for (const QVariant &v : loaded) {
//             appearance[i] = v.toInt();
//             // qDebug() << "Appearance preset value " << i << ": " << appearance[i];
//             i++;
//         }
//         current_soldier->ApplyAppearancePreset(appearance);
//         onSoldierSelected();
//     }
// }


void MainWindow::RevertSoldierClicked() {
    // qDebug() << "RevertSoldierClicked";
    if (current_soldier) {
        current_soldier->RevertChanges();
        onSoldierSelected();
    }
}

void MainWindow::RevertAllClicked() {
    // qDebug() << "RevertAllClicked";
    for (auto& pair : soldiers_to_save) {
        pair.second.RevertChanges();
    }
    soldiers_to_save.clear();
    onSoldierSelected();
}

void MainWindow::ExitButtonClicked() {
    // qDebug() << "ExitButtonClicked";
    ui.stackedWidget->setCurrentWidget(ui.SavePageWidget);
    current_soldier = nullptr;
    soldiers_to_save.clear();


}

void MainWindow::GenerateINIFile() {
    qDebug() << "Generating config file...";
    QSettings settings("config.ini", QSettings::IniFormat);
    QString home_path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    #ifdef Q_OS_WIN
        QString path = home_path + "/Documents/My Games/XCOM - Enemy Within/XComGame/SaveData/";
        qDebug() << "Windows OS detected";
    #elif defined(Q_OS_LINUX)
        QString path = home_path + "/.local/share/feral-interactive/XCOM/XEW/savedata/";
        qDebug() << "Linux OS detected";
    #elif defined(Q_OS_MACOS)
        QString path = home_path + "/Library/Application Support/Feral Interactive/XCOM Enemy Unknown/XEW/SaveData/";
        qDebug() << "Mac OS detected";
    #else
        QString path = "No idea where the save files are located on this OS";
        qDebug() << "Unknown OS detected";
    #endif

    settings.setValue("FIRST_RUN", true);
    qDebug() << "Setting FIRST_RUN to:" << settings.value("FIRST_RUN").toBool();
    settings.setValue("BACKUP_LIMIT", 10);
    qDebug() << "Setting BACKUP_LIMIT to:" << settings.value("BACKUP_LIMIT").toInt();
    settings.setValue("SAVE_DIR_PATH", path);
    qDebug() << "Setting SAVE_DIR_PATH to:" << settings.value("SAVE_DIR_PATH").toString();
    settings.setValue("APPEARANCE_PRESET_ENABLED", false);
    qDebug() << "Setting APPEARANCE_PRESET_ENABLED to:" << settings.value("APPEARANCE_PRESET_ENABLED").toBool();
    settings.setValue("AUTO_LOAD_LAST_PATH", false);
    qDebug() << "Setting AUTO_LOAD_LAST_PATH to:" << settings.value("AUTO_LOAD_LAST_PATH").toBool();
    settings.setValue("STAY_AFTER_SAVE", false);
    qDebug() << "Setting STAY_AFTER_SAVE to:" << settings.value("STAY_AFTER_SAVE").toBool();
    settings.setValue("VENT_BUTTON", false);
    qDebug() << "Setting VENT_BUTTON to:" << settings.value("VENT_BUTTON").toBool();
    QVariantList appearance_preset;
    appearance_preset << 46 << 2 << 0 << 3 << -1 << 0 << -1 << -1 << 0 << -1 << -1 << -1 << 0 << 0 << 0 << -1 << -1;
    settings.setValue("APPEARANCE_PRESET", appearance_preset);
    settings.sync();
    qDebug() << "Config file generated.";
}

void MainWindow::LoadINIFile() {
    // QSettings settings("config.ini", QSettings::IniFormat);
    backup_limit = settings.value("BACKUP_LIMIT", 10).toInt();
    save_dir_path = settings.value("SAVE_DIR_PATH", "Failed to load config.ini file").toString();
    stay_after_save = settings.value("STAY_AFTER_SAVE", false).toBool();

    if (settings.value("FIRST_RUN", false).toBool()) {
        settings.setValue("FIRST_RUN", false);
        settings.sync();
        QTimer::singleShot(0, this, [this] { QMessageBox::information(this, "First Launch!", "First app launch detected.\n To load available saves make sure to confirm path with the \"Load Path...\" button in the top right.\nIf you have any problems while using the app check out README.md"); });
    }
    else {
        settings.setValue("AUTO_LOAD_LAST_PATH", true);
    }
    if (settings.value("APPEARANCE_PRESET_ENABLED", false).toBool()) {
        ui.AppearanceButton->setEnabled(true);
        ui.AppearanceButton->show();
    }
    else {
        ui.AppearanceButton->setEnabled(false);
        ui.AppearanceButton->hide();
    }
    auto_load_last_path = settings.value("AUTO_LOAD_LAST_PATH", false).toBool();
    if (settings.value("VENT_BUTTON", false).toBool()) {
        ui.VentButton->setEnabled(true);
        ui.VentButton->show();
        ui.Vent1Box->setEnabled(true);
        ui.Vent1Box->show();
        ui.Vent2Box->setEnabled(true);
        ui.Vent2Box->show();
    }
    else {
        ui.VentButton->setEnabled(false);
        ui.VentButton->hide();
        ui.Vent1Box->setEnabled(false);
        ui.Vent1Box->hide();
        ui.Vent2Box->setEnabled(false);
        ui.Vent2Box->hide();
    }
    qDebug() << "Loaded settings from config file.";
}