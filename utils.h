#ifndef UTILS_H
#define UTILS_H

#include "json11.hpp"
#include "xcomsave/xcom.h"

#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace json11;

std::vector<std::string> splitString(const std::string& str, char delimiter);

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
            //this also need weirdness check.
            if (value >= 1) { enabled = true;}
            else { enabled = false;}
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

    }
    void Disable_Perk(int i)
    {
        if (perks[i].enabled)
        {
        perks[i].enabled = false;
        perks[i].value = 0;
        stats_diff -= perks[i].stats;
        }
    }
    std::array<std::string,3> Calculate_Stats() {
        std::string mobility_label = "<b>Mobility: "+ std::to_string(current_stats.mobility) + "</b>";

        if (stats_diff.mobility > 0) {
            mobility_label += "<b><font color='green'> +" + std::to_string(stats_diff.mobility) + "</font></b>";
        }
        else if (stats_diff.mobility < 0) {
            mobility_label += "<b><font color='red'> " + std::to_string(stats_diff.mobility) + "</font></b>";
        }

        std::string aim_label = "<b>Aim: " + std::to_string(current_stats.aim) + "</b>";

        if (stats_diff.aim > 0) {
            aim_label += "<b><font color='green'> +" + std::to_string(stats_diff.aim) + "</font></b>";
        }
        else if (stats_diff.aim < 0) {
            aim_label += "<b><font color='red'> " + std::to_string(stats_diff.aim) + "</font></b>";
        }

        std::string will_label = "<b>Will: " + std::to_string(current_stats.will) + "</b>";

        if (stats_diff.will > 0) {
            will_label += "<b><font color='green'> +" + std::to_string(stats_diff.will) + "</font></b>";
        }
        else if (stats_diff.will < 0) {
            will_label += "<b><font color='red'> " + std::to_string(stats_diff.will) + "</font></b>";
        }
        return {mobility_label, aim_label, will_label};   
    }
};

// xcomsave headers, since they were supposed to be main files and dont have own headers?
int xcom2json(std::string save_path, json11::Json& to_write);
int json2xcom(std::string save_path, json11::Json& to_write);

json11::Json load_json_file(const std::string& file_path);
std::vector<Perk> load_perks(json11::Json& json, int soldier_index);
void update_json(json11::Json& json, Soldier soldier);
void save_json_file(const std::string& file_path, json11::Json& json);

namespace Get_Soldiers
{
    std::string firstname(const json11::Json& entry, int soldier_index);
    //later on replace soldier_index with pointer to a soldier?
    std::string firstname(const xcom::saved_game& save, int soldier_index);
    std::string nickname(const json11::Json& entry, int soldier_index);
    std::string lastname(const json11::Json& entry, int soldier_index);
    std::string class_type(const json11::Json& entry, int soldier_index);
    int rank(const json11::Json& entry, int soldier_index);
    std::string eStatus(const json11::Json& entry, int soldier_index);
    std::vector<int> upgrades(const json11::Json& entry, int soldier_index);
    SoldierStats stats(json11::Json& json, int soldier_index);
    //Perks that give extra items have weird values based on other perks and items equipped.
    //This function uneqiups all items that might cause problems.
    //Items so far (Perk index - Item name - Item value):
    // 44 - Smoke Grenade - 82
    // 22 - Shredder Rocket - 89
    // 8 - Battle Scanner - 97
    void reset_arrSmallItems(json11::Json& json, int soldier_index);
}



#endif