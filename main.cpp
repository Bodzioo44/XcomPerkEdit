#include "MainWindow.h"
#include <QtWidgets/QApplication>

#include "utils.h"
#include "json11/json11.hpp"

#include <iostream>
#include <string>
#include <map>
using namespace std;
using namespace json11;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();

    // //reading json
    // Json json = load_json_file("../Testing/save475.json");
    // Json entry_list = json["checkpoints"][0]["checkpoint_table"];

    // //loading soldiers
    // map<string, int> soldier_map;
    // int i = 0;
    // for (const Json& entry : entry_list.array_items()) {
    //     //string temp; temp = entry.dump(); cout << temp << endl;
    //     //cout << "\n\n";
    //     //cout << entry["name"].string_value() << endl;
    //     if (entry["name"].string_value().find("XGStrategySoldier") != string::npos)
    //     {

    //         Json properties = entry["properties"];
    //         //Soldier is alive && Soldier has rank above specialist
    //         if (properties[3]["value"] != "eStatus_Dead" && properties[1]["properties"][4]["value"].int_value() > 1)
    //         {
    //             string soldier_firstname = properties[1]["properties"][1]["value"]["str"].string_value();
    //             string soldier_nickname = properties[1]["properties"][2]["value"]["str"].string_value();
    //             string soldier_lastname = properties[1]["properties"][3]["value"]["str"].string_value();
    //             string soldier_class = properties[1]["properties"][11]["properties"][0]["value"]["str"].string_value();
    //             int soldier_rank = properties[1]["properties"][4]["value"].int_value();
    //             //cout << "below: " << i << endl;
    //             cout << "Soldier: " << soldier_firstname << " " << soldier_nickname << " " << soldier_lastname << " " << soldier_rank << " " << soldier_class << endl;
                
    //             soldier_map[soldier_nickname] = i;
    //             //cout << "Looking for Soldiers with precision shot" << endl;
    //             //change_soldier_skill(json, i, 2);

    //         }
    //     }
    //     i++;
    // }
    // for (auto const& x : soldier_map)
    // {
    //     std::cout << x.first << " " << x.second << std::endl;
    // }
    // //select soldier

    // int selected_soldier = 281;

    // for(int i : Get_Soldiers::upgrades(json, selected_soldier))
    // {
    //     cout << i << ", ";
    // }
    // cout << endl;


    // // std::vector<Perk> perks = load_perks("sniper");
    // // for(const Perk& perk : perks)
    // // {
    // //     std::cout << perk;
    // // }




    // //std::cout << "Hello World!" << std::endl;
    // return 0;
}