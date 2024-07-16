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

    void operator += (const SoldierStats& statsy)
    {
        mobility = mobility + statsy.mobility;
        aim = aim + statsy.aim;
        will = will + statsy.will;
    }
    void operator -= (const SoldierStats& statsy)
    {
        //std::cout << "ayo: " << will << " - " << statsy.will << std::endl;
        mobility = mobility - statsy.mobility;
        aim = aim - statsy.aim;
        will = will - statsy.will;
    }
    friend std::ostream& operator<<(std::ostream& os, const SoldierStats& statsy)
    {
        os << "(" << "Mobility: " << statsy.mobility << ", Aim: " << statsy.aim << ", Will:" << statsy.will << ")";
        return os;
    }
};

struct Perk
{
    int index;
    int value;
    SoldierStats stats;
    bool enabled;

    Perk(int index, int value, SoldierStats stats)
        : index(index), value(value), stats(stats){
            if (value > 1)
            {
                std::fstream file;
                file.open("../Logs/logs.txt", std::ios::app);
                file << "value greater than 1: " << value << " for index: " << index << std::endl;
                file.close();
            }
            if (value >= 1) { enabled = true;}
            else { enabled = false;}
            //std::cout << "inside perk constructor: " << index << std::endl;
    }
    //TODO: figure out when the value is bigger than 1
    // void Enable(int i = 1)
    // {
    //     value = i;
    //     enabled = true;
    // }
    // void Disable()
    // {
    //     value = 0;
    //     enabled = false;
    // }

    
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
    int json_index;

    // Soldier(SoldierStats current_stats, std::vector<Perk> perks): current_stats(current_stats), perks(perks)
    // {}

    void Enable_Perk(int i)
    {
        if (!perks[i].enabled)
        {
            //TODO: do some skill check for potential weird values
            perks[i].enabled = true;
            perks[i].value = 1;

            stats_diff += perks[i].stats;
        }
        else
        {
            std::cout << "are we supposed tobe hre enabled: "<< i << std::endl;
        }

    }
    void Disable_Perk(int i)
    {
        if (perks[i].enabled)
        {
        perks[i].enabled = false;
        perks[i].value = 0;
        stats_diff -= perks[i].stats;
        }
        else
        {
            std::cout << "are we supposed tobe hre disabled: " << i << std::endl;
        }
    }
};

using perk_map = std::map<int, PerkAssets>;
perk_map load_perk_info(std::vector<Perk> perks);
json11::Json load_json_file(const std::string& file_path);
std::vector<Perk> load_perks(json11::Json& json, int soldier_index);
void json_update(json11::Json& json, Soldier soldier);

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