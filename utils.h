#ifndef UTILS_H
#define UTILS_H

#include "json11/json11.hpp"


#include <map>
#include <iostream>

struct Perk;


json11::Json load_json_file(const std::string& file_path);
//void change_soldier_skill(json11::Json& json, int soldier_index, int skill_id);
std::vector<Perk> load_perks(json11::Json& json, int soldier_index, std::string soldier_class);

struct Perk
{
    int index;
    int value;
    std::string name;

    Perk(int index, std::string name) : index(index), name(name) {
        value = 0;
    }
    std::string GetName() { return name; }
    int GetValue() { return value; }
    //TODO: fix this
    //some of the values are diffrent number that 1 for some reasond
    void SwitchValue() 
    {
        if (value == 0)
        {
            value = 1;
        }
        else
        {
            value = 0;
        }
    }
    friend std::ostream& operator<<(std::ostream& os, const Perk& perk)
    {
        os << "Perk: " << perk.name << " (Index: " << perk.index << ", Value: " << perk.value << ")";
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
    std::vector<int> upgrades(const json11::Json& entry, int soldier_index);
}


// enum Class
// {
//     Sniper,
//     Assault,
//     Scout,
//     Gunner,
//     Rocketeer,
//     Infantry,
//     Medic,
//     Engineer,
// };




#endif