#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    ui.setupUi(this);
    this->setCentralWidget(ui.centralwidget);
    ui.centralwidget->setLayout(ui.gridLayout);
    ui.PerkPage->setLayout(ui.PerkGridLayout);

    connect(ui.SoldierList, &QListWidget::itemSelectionChanged, this, &MainWindow::onSoldierSelected);
    connect(ui.PerkEditButton, &QPushButton::clicked, this, &MainWindow::PerkEditButtonClicked);
    connect(ui.SaveButton, &QPushButton::clicked, this, &MainWindow::SaveButtonClicked);
    
    for (int i = 0; i < 18; i++)
    {
        PerkButton* button = new PerkButton(this);
        ui.PerkGridLayout->addWidget(button, (i / 3) + 2, i % 3);
        perk_buttons.push_back(button);
        connect(button, &QToolButton::clicked, this, [this, i] { this->onPerkSelected(i); });
    }

    //TODO: Move this out of the constructor
    try
    {
        json = load_json_file("../Testing/save475.json");
        int i = 0;
        for (const Json& entry : json["checkpoints"][0]["checkpoint_table"].array_items())
        {
            //Checks if entry is a soldier
            if (entry["name"].string_value().find("XGStrategySoldier") != std::string::npos)
            {
                //Checks if soldier is alive and has rank above specialist
                if (Get_Soldiers::eStatus(json, i) != "eStatus_Dead" && Get_Soldiers::rank(json, i) > 1)
                {
                    string soldier_fullname = Get_Soldiers::firstname(json, i) + " '" + Get_Soldiers::nickname(json, i) + "' " + Get_Soldiers::lastname(json, i);
                    string icon_path = "../assets/icons/" + Get_Soldiers::class_type(json, i) + "_icon" + ".png";

                    QIcon icon(QString::fromStdString(icon_path));
                    QListWidgetItem* item = new QListWidgetItem(icon, QString::fromStdString(soldier_fullname));
                    ui.SoldierList->addItem(item);

                    index_translation[ui.SoldierList->count() - 1] = i; 
                }
            }
            i++;
        }
    }
    catch (std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void MainWindow::onSoldierSelected()
{
    int current_row = ui.SoldierList->currentRow(); //current row on the list
    int soldier_index = index_translation[current_row]; //soldier index in the save file
    int soldier_rank = Get_Soldiers::rank(json, soldier_index);

    SoldierStats stats = Get_Soldiers::stats(json, soldier_index);
    ui.MobilityLabel->setText(QString::fromStdString("<b>Mobility: " + to_string(stats.mobility) + "</b>"));
    ui.AimLabel->setText(QString::fromStdString("<b>Aim: " + to_string(stats.aim) + "</b>"));
    ui.WillLabel->setText(QString::fromStdString("<b>Will: " + to_string(stats.will) + "</b>"));

    vector<Perk> soldier_perks = load_perks(json, soldier_index); //vector of soldier perks (in order)
    perk_map perk_info = load_perk_info(soldier_perks); //map of perk index to perk data

    //grey out all other perks in the same row
    for (int i = 0; i < 18; i++)
    {
        const Perk& current_perk = soldier_perks[i];
        perk_buttons[i]->LoadPerk(perk_info[current_perk.index], current_perk);
        if (current_perk.value == 0)
        {
            perk_buttons[i]->GreyOut();
        }
        else
        {
            perk_buttons[i]->LightUp();
        }
        if ((i+1) > (soldier_rank-1)*3)
        {
            perk_buttons[i]->setDisabled(true);
        }
        else
        {
            perk_buttons[i]->setDisabled(false);
        }
    }

    current_soldier.perks = soldier_perks;
    current_soldier.current_stats = stats;
}

void MainWindow::onPerkSelected(int i)
{
    //grey out all other perks in the same row
    int start_index = (i / 3) * 3;
    for (int j = start_index; j < start_index + 3; j++)
    {
        if (j == i)
        {
            continue;
        }
        current_soldier.perks[j].SwitchValue();
        perk_buttons[j]->GreyOut();
    }

    current_soldier.perks[i].SwitchValue();
    perk_buttons[i]->LightUp();

    
}

void MainWindow::PerkEditButtonClicked()
{
    ui.stackedWidget->setCurrentWidget(ui.PerkPage);
}

void MainWindow::SaveButtonClicked()
{
    onSoldierSelected();
}