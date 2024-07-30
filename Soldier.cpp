#include "Soldier.h"

Soldier::Soldier(xcom::checkpoint* soldier = nullptr): properties(&soldier->properties) {
    starting_stats = GetSoldiers::stats(properties);
    difference_stats = SoldierStats();
    perks = GetSoldiers::perks(properties);
}

void Soldier::EnablePerk(int index) {
    Perk& perk = perks[index];
    if (!perk.enabled) {
        perk.enabled = true;
        perk.value++;
        difference_stats += perk.stats;
    }
}

void Soldier::DisablePerk(int index) {
    Perk& perk = perks[index];
    if (perk.enabled) {
        perk.enabled = false;
        perk.value--;
        difference_stats -= perk.stats;
    }
}

xcom::property_list* Soldier::GetPropertyList() const{
    return properties;
}

LabelSet Soldier::GetLabels() const{
        std::string mobility_label = "<b>Mobility: "+ std::to_string(starting_stats.mobility) + "</b>";
        std::string aim_label = "<b>Aim: " + std::to_string(starting_stats.aim) + "</b>";
        std::string will_label = "<b>Will: " + std::to_string(starting_stats.will) + "</b>";

        if (difference_stats.mobility > 0) {
            mobility_label += "<b><font color='green'> +" + std::to_string(difference_stats.mobility) + "</font></b>";
        }
        else if (difference_stats.mobility < 0) {
            mobility_label += "<b><font color='red'> " + std::to_string(difference_stats.mobility) + "</font></b>";
        }
        if (difference_stats.aim > 0) {
            aim_label += "<b><font color='green'> +" + std::to_string(difference_stats.aim) + "</font></b>";
        }
        else if (difference_stats.aim < 0) {
            aim_label += "<b><font color='red'> " + std::to_string(difference_stats.aim) + "</font></b>";
        }
        if (difference_stats.will > 0) {
            will_label += "<b><font color='green'> +" + std::to_string(difference_stats.will) + "</font></b>";
        }
        else if (difference_stats.will < 0) {
            will_label += "<b><font color='red'> " + std::to_string(difference_stats.will) + "</font></b>";
        }
        return {mobility_label, aim_label, will_label};   
}

PerkSet Soldier::GetPerks() const {
    return perks;
}

void Soldier::UpdateSoldier() {
    //Updating Perks
    xcom::struct_property& m_kChar = static_cast<xcom::struct_property&> (*properties->at(0));
    xcom::static_array_property& aUpgrades = static_cast<xcom::static_array_property&> (*m_kChar.properties[3]);
    for (const Perk& perk : perks) {
        static_cast<xcom::int_property*> (aUpgrades.properties[perk.index].get())->value = perk.value;
    }
    //Updating Stats
    xcom::static_array_property& aStats = static_cast<xcom::static_array_property&> (*m_kChar.properties[6]);
    static_cast<xcom::int_property*> (aStats.properties[3].get())->value = starting_stats.mobility + difference_stats.mobility;
    static_cast<xcom::int_property*> (aStats.properties[1].get())->value = starting_stats.aim + difference_stats.aim;
    static_cast<xcom::int_property*> (aStats.properties[7].get())->value = starting_stats.will + difference_stats.will;
}

void Soldier::RevertChanges() {
    perks = GetSoldiers::perks(properties);
    difference_stats = {0, 0, 0};
}

namespace GetSoldiers {
    std::string class_type(const xcom::property_list* properties) {
        xcom::struct_property& m_kSoldier = static_cast<xcom::struct_property&> (*properties->at(1));
        xcom::struct_property& kClass = static_cast<xcom::struct_property&> (*m_kSoldier.properties[11]);
        return static_cast<xcom::string_property&> (*kClass.properties[0]).str.str;
    }
    std::string full_name(const xcom::property_list* properties) {
        xcom::struct_property& m_kSoldier = static_cast<xcom::struct_property&> (*properties->at(1));
        xcom::string_property& strFirstName = static_cast<xcom::string_property&> (*m_kSoldier.properties[1]);
        xcom::string_property& strLastName = static_cast<xcom::string_property&> (*m_kSoldier.properties[2]);
        xcom::string_property& strNickName = static_cast<xcom::string_property&> (*m_kSoldier.properties[3]);
        return strFirstName.str.str + " \"" + strNickName.str.str + "\" " + strLastName.str.str;
    }
    //TODO: check if this works?
    std::string eStatus(const xcom::property_list* properties) {
        xcom::enum_property& m_eStatus = static_cast<xcom::enum_property&> (*properties->at(3));
        return m_eStatus.value.name;
    }

    int rank(const xcom::property_list* properties) {
        xcom::struct_property& m_kSoldier = static_cast<xcom::struct_property&> (*properties->at(1));
        return static_cast<xcom::int_property&> (*m_kSoldier.properties[4]).value;
    }

    PerkSet perks(const xcom::property_list* properties) {
        xcom::struct_property& m_kChar = static_cast<xcom::struct_property&> (*properties->at(0));
        xcom::static_array_property& aUpgrades = static_cast<xcom::static_array_property&> (*m_kChar.properties[3]);

        std::string path = ":/assets/" + class_type(properties) + ".txt";
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + path);
        }

        PerkSet perks;
        std::string line;
        int i = 0;
        //loads perks from the file in format: index, mobility, aim, will
        //same perk can give different stats for different classes.
        while(std::getline(file, line)) {
            std::vector<std::string> row;
            std::stringstream ss(line);
            std::string value;
            while (std::getline(ss, value, ',')) {
                row.push_back(value);
            }
            //pulls index value from the save file
            int index_value = static_cast<xcom::int_property*> (aUpgrades.properties[std::stoi(row[0])].get())->value;
            perks[i] = Perk(std::stoi(row[0]), index_value, SoldierStats(std::stoi(row[1]), std::stoi(row[2]), std::stoi(row[3])));
            i++;
        }
        return perks;
    }

    SoldierStats stats(const xcom::property_list* properties) {
        xcom::struct_property& m_kChar = static_cast<xcom::struct_property&> (*properties->at(0));
        xcom::static_array_property& aStats = static_cast<xcom::static_array_property&> (*m_kChar.properties[6]);
        return SoldierStats(static_cast<xcom::int_property&> (*aStats.properties[3]).value, static_cast<xcom::int_property&> (*aStats.properties[1]).value, static_cast<xcom::int_property&> (*aStats.properties[7]).value);
    }
}