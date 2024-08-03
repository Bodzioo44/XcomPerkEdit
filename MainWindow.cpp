#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {
    ui.setupUi(this);
    this->setCentralWidget(ui.centralwidget);
    ui.centralwidget->setLayout(ui.gridLayout);
    ui.PerkEditPage->setLayout(ui.horizontalLayout);
    ui.SavePageWidget->setLayout(ui.SavePageVBoxLayout);
    ui.stackedWidget->setCurrentWidget(ui.SavePageWidget);

    connect(ui.SoldierListWidget, &QListWidget::currentRowChanged, this, &MainWindow::onSoldierSelected);
    connect(ui.SaveListWidget, &QListWidget::itemActivated, this, &MainWindow::onSaveSelected);
    connect(ui.SaveFileButton, &QPushButton::clicked, this, &MainWindow::SaveButtonClicked);
    connect(ui.SelectPathButton, &QPushButton::clicked, this, &MainWindow::SelectPathButtonClicked);
    connect(ui.ExitButton, &QPushButton::clicked, this, &MainWindow::ExitButtonClicked);
    connect(ui.RevertSoldierButton, &QPushButton::clicked, this, &MainWindow::RevertSoldierClicked);
    connect(ui.RevertAllButton, &QPushButton::clicked, this, &MainWindow::RevertAllClicked);

    std::vector<QHBoxLayout*> rows = { ui.Row1HBoxLayout, ui.Row2HBoxLayout, ui.Row3HBoxLayout, ui.Row4HBoxLayout, ui.Row5HBoxLayout, ui.Row6HBoxLayout };
    for (int i = 0; i < 18; i++) {
        PerkButton* button = new PerkButton(this);
        rows[i / 3]->addWidget(button);
        perk_buttons.push_back(button);
        connect(button, &QToolButton::clicked, this, [this, i] { this->onPerkSelected(i); });
    }

    if (!QFile("config.ini").exists()) {
        GenerateINIFile();
    }
    LoadINIFile();
    ui.PathLineEdit->setText(save_dir_path);
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
    ui.SoldierListWidget->clear();

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
                std::string full_name = GetSoldiers::full_name(properties);
                std::string icon_path = ":/assets/icons/" + GetSoldiers::class_type(properties) + "_icon.png";

                QIcon icon (QString::fromStdString(icon_path));
                QListWidgetItem* item = new QListWidgetItem(icon, QString::fromStdString(full_name));
                ui.SoldierListWidget->addItem(item);

                soldier_index_translation[ui.SoldierListWidget->count() - 1] = i;
            }
        }
        i++;
    }
    ui.SoldierListWidget->setFixedWidth(ui.SoldierListWidget->sizeHint().width() + 50);
    if (ui.SoldierListWidget->count() == 0) {
        QMessageBox::warning(this, "No soldiers found", "No acceptable soldiers were found in the save file.");
        return;
    }
    ui.stackedWidget->setCurrentWidget(ui.PerkEditPage);
    qDebug() << "Checkpoint table loaded.";
    //to always trigger onSoldierSelected() when the save is loaded.
    ui.SoldierListWidget->setCurrentRow(0);
}

//QListWidget::currentRowChanged will also trigger on QListWidget::clear().
void MainWindow::onSoldierSelected() {
    qDebug() << "Soldier selected.";
    if (ui.SoldierListWidget->currentRow() == -1) {
        qDebug() << "Invalid row index, probably due to QListWidgeT::clear()";
        return;
    }
    int current_row = ui.SoldierListWidget->currentRow(); //current row on the list
    int soldier_index = soldier_index_translation[current_row]; //soldier index in the save file
    qDebug() << "Soldier selected with row: " << current_row << " Save index: " << soldier_index;

    if (soldiers_to_save.find(soldier_index) == soldiers_to_save.end()) {
        soldiers_to_save[soldier_index] = Soldier(&checkpoint_table_ptr->at(soldier_index));
        //soldiers_to_save.emplace(soldier_index, &checkpoint_table_ptr->at(soldier_index));
        //soldiers_to_save.insert_or_assign(soldier_index, &checkpoint_table_ptr->at(soldier_index));
    }
    current_soldier = &soldiers_to_save[soldier_index];
    int soldier_rank = GetSoldiers::rank(current_soldier->GetPropertyList());
    qDebug() << "Soldier rank: " << soldier_rank;
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
    // qDebug() << "Soldier successfully loaded!";
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
    qDebug() << "Saving...";
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
        ui.SaveListWidget->clearSelection();
        ui.stackedWidget->setCurrentWidget(ui.SavePageWidget);
    }
    else {
        QMessageBox::warning(this, "No soldiers selected", "No soldiers were selected for editing.");
    }
}

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
    settings.sync();
    qDebug() << "Config file generated.";
}

void MainWindow::LoadINIFile() {
    QSettings settings("config.ini", QSettings::IniFormat);
    backup_limit = settings.value("BACKUP_LIMIT", 10).toInt();
    save_dir_path = settings.value("SAVE_DIR_PATH", "Failed to load config.ini file").toString();
    if (settings.value("FIRST_RUN", false).toBool()) {
        settings.setValue("FIRST_RUN", false);
        settings.sync();
        QTimer::singleShot(0, this, [this] { QMessageBox::information(this, "First Launch!", "First app launch detected.\n To load available saves make sure to confirm path with the \"Load Path...\" button in the top right.\nIf you have any problems while using the app check out USAGE.md"); });
    }
    qDebug() << "Loaded settings from config file.";
}