#ifndef UTILS_H
#define UTILS_H

#include "json11/json11.hpp"

#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

struct PerkAssets
{
    std::string name;
    std::string description;
    std::string icon_path;
};

struct SoldierStats
{
    int mobility;
    int aim;
    int will;

    SoldierStats() 
        : mobility(0), aim(0), will(0) {}
    SoldierStats(int mobility, int aim, int will)
        : mobility(mobility), aim(aim), will(will) {}
};


struct Perk
{
    int index;
    int value;
    SoldierStats stats;

    Perk(int index, int value, SoldierStats stats)
        : index(index), value(value), stats(stats){
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
        os << "(Index: " << perk.index << ", Value: " << perk.value << ", Mobility: " << perk.stats.mobility << ", Aim: " << perk.stats.aim << ", Will: " << perk.stats.will<< ")";
        return os;
    }
};

struct Soldier
{
    SoldierStats stats_diff;
    SoldierStats current_stats;
    std::vector<Perk> perks;
};

using perk_map = std::map<int, PerkAssets>;
perk_map load_perk_info(std::vector<Perk> perks);
json11::Json load_json_file(const std::string& file_path);
std::vector<Perk> load_perks(json11::Json& json, int soldier_index);


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
    SoldierStats stats(json11::Json& json, int soldier_index);
}



#endif