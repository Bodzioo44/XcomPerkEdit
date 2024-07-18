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

    json11::Json json;
    int err = xcom2json(file_path, json);
    if (err != 0) {
        throw std::runtime_error("Could not load file " + file_path);
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

json11::Json update_json(json11::Json& json, Soldier soldier)
{
    //ugly af
    //this is the only way to update the json object without switching to a different library, or heavily modifying the current one?
    //step by step turning the json into map/vector objects, updating the values, and then turning them back into json objects
    json11::Json::object new_json = json.object_items();
    json11::Json::array step1_checkpoints = new_json["checkpoints"].array_items();
    json11::Json::object step2_0 = step1_checkpoints[0].object_items();
    json11::Json::array step3_checkpoint_table = step2_0["checkpoint_table"].array_items();
    json11::Json::object step4_soldier_index = step3_checkpoint_table[soldier.json_index].object_items();
    json11::Json::array step5_properties = step4_soldier_index["properties"].array_items();
    json11::Json::object step6_0 = step5_properties[0].object_items();
    json11::Json::array step7_properties = step6_0["properties"].array_items();

    //pulling the perks array from the json, and updating it
    json11::Json::object step8_3 = step7_properties[3].object_items();
    json11::Json::array step9_int_values_3 = step8_3["int_values"].array_items();
    for (const Perk& p : soldier.perks)
    {
        step9_int_values_3[p.index] = p.value;
    }

    //pulling the stats array from the json, and updating it
    json11::Json::object step8_6 = step7_properties[6].object_items();
    json11::Json::array step9_int_values_6 = step8_6["int_values"].array_items();
    step9_int_values_6[1] = soldier.current_stats.aim + soldier.stats_diff.aim;
    step9_int_values_6[3] = soldier.current_stats.mobility + soldier.stats_diff.mobility;
    step9_int_values_6[7] = soldier.current_stats.will + soldier.stats_diff.will;


    //saving the updated perks
    step8_3["int_values"] = step9_int_values_3;
    json11::Json new_step8_3 = step8_3;
    step7_properties[3] = new_step8_3;

    //saving the updated stats
    step8_6["int_values"] = step9_int_values_6;
    json11::Json new_step8_6 = step8_6;
    step7_properties[6] = new_step8_6;



    json11::Json new_step7_properties = step7_properties;
    step6_0["properties"] = new_step7_properties;

    json11::Json new_step6_0 = step6_0;
    step5_properties[0] = new_step6_0;

    json11::Json new_step5_properties = step5_properties;
    step4_soldier_index["properties"] = new_step5_properties;

    json11::Json new_step4_soldier_index = step4_soldier_index;
    step3_checkpoint_table[soldier.json_index] = new_step4_soldier_index;

    json11::Json new_step3_checkpoint_table = step3_checkpoint_table;
    step2_0["checkpoint_table"] = new_step3_checkpoint_table;

    json11::Json new_step2_0 = step2_0;
    step1_checkpoints[0] = new_step2_0;

    json11::Json new_step1_checkpoints = step1_checkpoints;
    new_json["checkpoints"] = new_step1_checkpoints;
    json11::Json new_json_json = new_json;

    return new_json_json;
}

void save_json_file(const std::string& file_path, json11::Json& json)
{
    std::ofstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file " + file_path);
    }
    file << json.dump();
    file.close();
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