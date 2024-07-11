#include <fstream>

#include <iostream>
#include <cassert>
#include <cstring>
#include <string>
#include <array>
#include <vector>
#include <sstream>
#include "utils.h"

json11::Json load_json_file(const std::string& file_path) {

    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file " + file_path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string json_str = buffer.str();

    std::string err;
    //std::cout << json_str << std::endl;
    json11::Json json = json11::Json::parse(json_str, err);
    if (!err.empty()) {
        throw std::runtime_error("Failed to parse JSON: " + err);
    }

    return json;
}

//Probably useless, its better to change it once
// void change_soldier_skill(json11::Json& json, int soldier_index, int skill_index)
// {
//     auto skill = json["checkpoints"][0]["checkpoint_table"][soldier_index]["properties"][0]["properties"][3]["int_values"][skill_index].int_value();
//     if (skill)
//     {
//         std::cout << "Skill detected" << std::endl;
//     }
//     else
//     {
//         std::cout << "Skill not detected" << std::endl;
//     }
// }



std::vector<Perk> load_perks(json11::Json& json, int soldier_index, std::string soldier_class)
{
    //holy spaghetti
    //checkpoints[0].checkpoint_table[284].properties[1].properties[11].properties[0].value.str
    //std::string soldier_class = json["checkpoints"][0]["checkpoint_table"][soldier_index]["properties"][1]["properties"][11]["properties"][0]["value"]["str"].string_value();
    //std::cout << soldier_class << std::endl;

    std::vector<int> upgrades = Get_Soldiers::upgrades(json, soldier_index);

    std::vector<Perk> perks;
    std::string path = "../assets/" + soldier_class + ".txt";
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file " + path);
    }
    std::string line;
    while (std::getline(file, line))
    {
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string value;

        while (std::getline(ss, value, ','))
        {
            row.push_back(value);
        }
        perks.push_back(Perk(std::stoi(row[0]), upgrades[std::stoi(row[0])], std::stoi(row[1]), std::stoi(row[2]), std::stoi(row[3])));
    }
    return perks;
}


// //0,1,2 - which skill at what rank from 1-7
// //nah, its easier to load 18 element array
// std::array<int, 18> load_soldier(json11::Json& json, std::string soldier_class, int rank)
// {
//     std::string path = "../assets/" + soldier_class + ".txt";

// }


// struct Perk
// {
//     int index;
//     int value;
//     std::string name;

//     Perk(int index, int value, std::string name) : index(index), value(value), name(name) {}
//     std::string GetName() { return name; }
//     int GetValue() { return value; }
//     void SwitchValue() 
//     {
//         if (value == 0)
//         {
//             value = 1;
//         }
//         else
//         {
//             value = 0;
//         }
//     }
// };


//soldier_index is the index of the soldier in the checkpoint_table (NOT the XGStrategySoldier index)
//json is the whole save file
namespace Get_Soldiers
{
    std::string firstname(const json11::Json& json, int soldier_index)
    {
        return json["checkpoints"][0]["checkpoint_table"][soldier_index]["properties"][1]["properties"][1]["value"]["str"].string_value();
    }
    std::string nickname(const json11::Json& json, int soldier_index)
    {
        return json["checkpoints"][0]["checkpoint_table"][soldier_index]["properties"][1]["properties"][3]["value"]["str"].string_value();
    }
    std::string lastname(const json11::Json& json, int soldier_index)
    {
        return json["checkpoints"][0]["checkpoint_table"][soldier_index]["properties"][1]["properties"][2]["value"]["str"].string_value();
    }
    std::string class_type(const json11::Json& json, int soldier_index)
    {
        return json["checkpoints"][0]["checkpoint_table"][soldier_index]["properties"][1]["properties"][11]["properties"][0]["value"]["str"].string_value();
    }
    int rank(const json11::Json& json, int soldier_index)
    {
        return json["checkpoints"][0]["checkpoint_table"][soldier_index]["properties"][1]["properties"][4]["value"].int_value();
    }
    std::string eStatus(const json11::Json& json, int soldier_index)
    {
        return json["checkpoints"][0]["checkpoint_table"][soldier_index]["properties"][3]["value"].string_value();
    }
    std::vector<int> upgrades(const json11::Json& json, int soldier_index)
    {
        std::vector<int> upgrades;
        for (const json11::Json& i : json["checkpoints"][0]["checkpoint_table"][soldier_index]["properties"][0]["properties"][3]["int_values"].array_items())
        {
            upgrades.push_back(i.int_value());
        }
        return upgrades;
    }
}