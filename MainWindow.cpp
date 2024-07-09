#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    ui.setupUi(this);
    this->setCentralWidget(ui.centralwidget);
    ui.centralwidget->setLayout(ui.gridLayout);
    // ui.PerkPageVerticalLayout->addLayout(ui.LabelLayout);
    // ui.PerkPageVerticalLayout->addLayout(ui.PerkGridLayout);

    

    //ui.stackedWidget->setCurrentWidget(ui.page_2);

    connect(ui.SoldierList, &QListWidget::itemSelectionChanged, this, &MainWindow::onSoldierSelected);
    connect(ui.PerkEditButton, &QPushButton::clicked, this, &MainWindow::PerkEditButtonClicked);
    
    // QLabel* will_label = new QLabel("Will: 0/0", this);
    // QLabel* aim_label = new QLabel("Aim: 0/0", this);
    // QLabel* mobility_label = new QLabel("Mobility: 0/0", this);
    // ui.PerkGridLayout->addWidget(will_label, 0, 0);
    // ui.PerkGridLayout->addWidget(aim_label, 0, 1);
    // ui.PerkGridLayout->addWidget(mobility_label, 0, 2);    
    
    for (int i = 0; i < 18; i++)
    {
        QToolButton* button = new QToolButton(this);
        ui.PerkGridLayout->addWidget(button, (i / 3), i % 3);
        connect(button, &QToolButton::clicked, this, [this, i] { this->onPerkSelected(i); });
    }

    // ui.PerkPageVerticalLayout->addLayout(ui.LabelLayout);
    // ui.PerkPageVerticalLayout->addLayout(ui.PerkGridLayout);
    // ui.PerkPageVerticalLayout->setStretch(0, 2); // For LabelLayout
    // ui.PerkPageVerticalLayout->setStretch(1, 10); // For PerkGridLayout
    

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
    for (const Perk& perk : soldier_perks)
    {
        cout << perk << endl;
    }
}


void MainWindow::onPerkSelected(int i)
{
    cout << i << endl;
}

void MainWindow::PerkEditButtonClicked()
{
    ui.stackedWidget->setCurrentWidget(ui.PerkPage);
}