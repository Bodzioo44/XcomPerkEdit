#include "utils.h"

std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream strStream(str);
    std::string token;

    while (std::getline(strStream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
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

json11::Json load_json_file(const std::string& file_path) {
    json11::Json json;
    int err = xcom2json(file_path, json);
    if (err != 0) {
        throw std::runtime_error("Could not load file " + file_path);
    }
    return json;
}

void save_json_file(const std::string& file_path, json11::Json& json)
{
    int err = json2xcom(file_path, json);
    if (err != 0) {
        throw std::runtime_error("Could not save file " + file_path);
    }
}


void update_json(json11::Json& json, Soldier soldier)
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
    json = json11::Json(new_json);
}



//soldier_index is the index of the soldier in the checkpoint_table (NOT the XGStrategySoldier index)
//json is the whole save file
namespace Get_Soldiers
{
    std::string firstname(const json11::Json& json, int soldier_index)
    {
        return json["checkpoints"][0]["checkpoint_table"][soldier_index]["properties"][1]["properties"][1]["value"]["str"].string_value();
    }

    std::string firstname(const xcom::saved_game& save, int soldier_index)
    {
        const xcom::checkpoint_chunk_table& checkpoint_chunk_table = save.checkpoints;
        const xcom::checkpoint_chunk& checkpoint_chunk = checkpoint_chunk_table[0];
        const xcom::checkpoint_table& checkpoint_table = checkpoint_chunk.checkpoints;
        const xcom::checkpoint& soldier = checkpoint_table[soldier_index];
        const xcom::property_list& properties = soldier.properties;
        const xcom::struct_property* m_kSoldier = static_cast<xcom::struct_property*> (properties[1].get());
        const xcom::string_property* strFirstName = static_cast<xcom::string_property*> (m_kSoldier->properties[1].get());
        return strFirstName->str.str;

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
    //needs work
    //checkpoints[0].checkpoint_table[260].properties[0].properties[2].properties[4].int_values[0]
    //eStatus + 12 -> "properties" -> [4] -> "int_values" for the backed up (inventory that soldier will equip whenever hes back to health?)
    //upgrades - 1 -> "properties" -> [4] -> "int_vales" for current inventory?

    //checkpoints[0].checkpoint_table[260].properties[15].properties[4].int_values()
    //return checkpoints[0].checkpoint_table[260].properties[0].properties[2] (same lvl as upgrades)
    //return checkpoints[0].checkpoint_table[260].properties[15]
    //checkpoints[0].checkpoint_table[260].properties[0].properties[2].properties[4].int_values()
    void reset_arrSmallItems(json11::Json& json, int soldier_index)
    {

    }
}