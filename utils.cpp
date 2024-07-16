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
    json11::Json json = json11::Json::parse(json_str, err);
    if (!err.empty()) {
        throw std::runtime_error("Failed to parse JSON: " + err);
    }
    return json;
}

std::vector<Perk> load_perks(json11::Json& json, int soldier_index)
{
    std::vector<int> upgrades = Get_Soldiers::upgrades(json, soldier_index);
    std::string soldier_class = Get_Soldiers::class_type(json, soldier_index);

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
        
        perks.push_back(Perk(std::stoi(row[0]), upgrades[std::stoi(row[0])], SoldierStats(std::stoi(row[1]), std::stoi(row[2]), std::stoi(row[3]))));
    }
    return perks;
}

perk_map load_perk_info(std::vector<Perk> perks)
{
    std::vector<int> perks_indexes;
    for (const Perk& p: perks)
    {
        perks_indexes.push_back(p.index);
    }
    std::sort(perks_indexes.begin(), perks_indexes.end());

    perk_map all_perks;
    std::string path = "../assets/All_Perk_Data.txt";
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file " + path);
    }
    std::string line;
    int i = 1;
    unsigned int current_perk = 0; //compiler was complaining about comparing signed and unsigned
    bool is_correct = false;
    PerkAssets icon_assets;

    while (std::getline(file,line))
    {
        if (i % 4 == 1 && std::stoi(line) == perks_indexes[current_perk])
        {
            is_correct = true;
        }
        else if (is_correct && i % 4 == 2)
        {
            icon_assets.name = line;
        }
        else if (is_correct && i % 4 == 3)
        {
            icon_assets.description = line;
        }
        else if (is_correct && i % 4 == 0)
        {
            icon_assets.icon_path = line;
            all_perks[perks_indexes[current_perk]] = icon_assets;
            current_perk++;
            is_correct = false;
            if (current_perk == perks_indexes.size())
            {
                break;
            }
        }
        i++;
    }
    return all_perks;
}

void json_update(json11::Json& json, Soldier soldier)
{
    std::cout << "For soldier: " << Get_Soldiers::nickname(json, soldier.json_index) << std::endl;
    std::cout << "Stats diff: " << soldier.stats_diff << std::endl;

    json11::Json new_json;
}

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
    SoldierStats stats(json11::Json& json, int soldier_index)
    {
        SoldierStats stats;
        stats.mobility = json["checkpoints"][0]["checkpoint_table"][soldier_index]["properties"][0]["properties"][6]["int_values"][3].int_value();
        stats.aim = json["checkpoints"][0]["checkpoint_table"][soldier_index]["properties"][0]["properties"][6]["int_values"][1].int_value();
        stats.will = json["checkpoints"][0]["checkpoint_table"][soldier_index]["properties"][0]["properties"][6]["int_values"][7].int_value();
        return stats;
    }
}