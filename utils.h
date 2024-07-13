#ifndef UTILS_H
#define UTILS_H

#include "json11/json11.hpp"
#include "Qt/PerkButton.h"

#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using perk_data = std::array<std::string, 3>;
using perk_map = std::map<int, perk_data>;
using soldier_stats = std::array<int, 3>;

struct Perk;
json11::Json load_json_file(const std::string& file_path);
std::vector<Perk> load_perks(json11::Json& json, int soldier_index);

perk_map load_perk_info(std::vector<Perk> perks);

struct Perk
{
    int index;
    int value;
    int mobility;
    int aim;
    int will;

    Perk(int index, int value, int mobility, int aim, int will)
        : index(index), value(value), mobility(mobility), aim(aim), will(will){
            if (value > 1)
            {
                std::fstream file;
                file.open("../Logs/logs.txt", std::ios::app);
                file << "value greater than 1: " << value << " for index: " << index << std::endl;
                file.close();
            }
    }
    //TODO: figure out when the value is bigger than 1
    void SwitchValue(int new_value = 1) 
    {
        if (value == 0)
        {
            value = new_value;
        }
        else
        {
            value = 0;
        }
    }
    friend std::ostream& operator<<(std::ostream& os, const Perk& perk)
    {
        os << "(Index: " << perk.index << ", Value: " << perk.value << ", Mobility: " << perk.mobility << ", Aim: " << perk.aim << ", Will: " << perk.will << ")";
        return os;
    }
};


namespace Get_Soldiers
{
    std::string firstname(const json11::Json& entry, int soldier_index);
    std::string nickname(const json11::Json& entry, int soldier_index);
    std::string lastname(const json11::Json& entry, int soldier_index);
    std::string class_type(const json11::Json& entry, int soldier_index);
    int rank(const json11::Json& entry, int soldier_index);
    std::string eStatus(const json11::Json& entry, int soldier_index);
    //TODO: dont pass the whole array, just the ones we need (optimalization)
    std::vector<int> upgrades(const json11::Json& entry, int soldier_index);
    soldier_stats load_stats(json11::Json& json, int soldier_index);
}



#endif