#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    ui.setupUi(this);
    this->setCentralWidget(ui.centralwidget);
    ui.centralwidget->setLayout(ui.gridLayout);
    ui.PerkPage->setLayout(ui.PerkGridLayout);

    connect(ui.SoldierList, &QListWidget::itemSelectionChanged, this, &MainWindow::onSoldierSelected);
    connect(ui.PerkEditButton, &QPushButton::clicked, this, &MainWindow::PerkEditButtonClicked);
    
    for (int i = 0; i < 18; i++)
    {
        PerkButton* button = new PerkButton(this, i);
        ui.PerkGridLayout->addWidget(button, (i / 3) + 2, i % 3);
        perk_buttons.push_back(button);
        connect(button, &QToolButton::clicked, this, [this, i] { this->onPerkSelected(i); });
    }

    //move this out of the constructor
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
    //cout << ui.SoldierList->currentRow() << endl;
    // cout << index_translation[ui.SoldierList->currentRow()] << endl;
    // cout << Get_Soldiers::class_type(json, 273);
    // cout << Get_Soldiers::class_type(json, index_translation[ui.SoldierList->currentRow()]) << endl;
    vector<Perk> soldier_perks = load_perks(json, index_translation[ui.SoldierList->currentRow()], Get_Soldiers::class_type(json, index_translation[ui.SoldierList->currentRow()]));
    soldier_stats stats = Get_Soldiers::load_stats(json, index_translation[ui.SoldierList->currentRow()]);
    //cout << stats[0] << " " << stats[1] << " " << stats[2] << endl;
    ui.MobilityLabel->setText(QString::fromStdString("Mobility: " + to_string(stats[0])));
    ui.AimLabel->setText(QString::fromStdString("Aim: " + to_string(stats[1])));
    ui.WillLabel->setText(QString::fromStdString("Will: " + to_string(stats[2])));
    // for (const Perk& perk : soldier_perks)
    // {
    //     cout << perk << endl;
    // }
    perk_map perk_info = load_perk_info(soldier_perks);


    //perk_info returns map [perk_index] = [perk_name, perk_description, perk_image]
    //bit convoluted

    int i = 0;
    for (const auto& [index, perk] : perk_info)
    {
        //cout << index << ": " << perk[0] << " " << perk[1] << " " << perk[2] <<  endl;
        //perk_buttons[index]->ChangeIcon("../assets/icons/" + perk + ".png");
        perk_buttons[i]->LoadPerk(perk_info[soldier_perks[i].index]);
        i++;
    }
}


void MainWindow::onPerkSelected(int i)
{
    cout << i << endl;
    perk_buttons[i]->GreyedOutSwitch();
}

void MainWindow::PerkEditButtonClicked()
{
    ui.stackedWidget->setCurrentWidget(ui.PerkPage);
    
}