#include "Soldier.h"

Soldier::Soldier(xcom::checkpoint* soldier = nullptr): properties(&soldier->properties) {
    starting_stats = GetSoldiers::stats(properties);
    appearance = GetSoldiers::appearance(properties);
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

QString Soldier::GetLabels() const{
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
        return QString::fromStdString(mobility_label + "<br>" + aim_label + "<br>" + will_label);
}

PerkSet Soldier::GetPerks() const {
    return perks;
}

AppearanceSet Soldier::GetAppearance() const {
    return appearance;
}

//Default values
//Some special soldiers (The General, Freaky) dont have full set of kAppearance.properties before joining xcom.
//however it shouldnt matter since they are not stored in XGStrategySoldier struct, thus not being detected.
//example int_property: { "name": "iHead", "kind": "IntProperty", "value": 148 }
void Soldier::ApplyAppearancePreset(AppearanceSet preset) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 21);

    appearance[0] = preset[0]; //iHead
    appearance[1] = preset[1]; //iGender
    appearance[2] = preset[2]; //iRace
    appearance[3] = preset[3]; //iHaircut
    if (preset[4] == -1) {
        appearance[4] = dist(gen); //iHairColor randomized
    }
    else {
        appearance[4] = preset[4]; //iHairColor
    }
    appearance[5] = preset[5]; //iFacialHair
    appearance[6] = preset[6]; //iBody
    appearance[7] = preset[7]; //iBodyMaterial
    appearance[8] = preset[8]; //iSkinColor
}

QDebug operator<<(QDebug s, const AppearanceSet& set) {
    s << "AppearanceSet:\n";
    s << "iHead: " << set[0] << "\n";
    s << "iGender: " << set[1] << "\n";
    s << "iRace: " << set[2] << "\n";
    s << "iHaircut: " << set[3] << "\n";
    s << "iHairColor: " << set[4] << "\n";
    s << "iFacialHair: " << set[5] << "\n";
    s << "iBody: " << set[6] << "\n";
    s << "iBodyMaterial: " << set[7] << "\n";
    s << "iSkinColor: " << set[8] << "\n";
    s << "iEyeColor: " << set[9] << "\n";
    s << "iFlag: " << set[10] << "\n";
    s << "iArmorSkin: " << set[11] << "\n";
    s << "iVoice: " << set[12] << "\n";
    s << "iLanguage: " << set[13] << "\n";
    s << "iAttitude: " << set[14] << "\n";
    s << "iArmorDeco: " << set[15] << "\n";
    s << "iArmorTint: " << set[16] << "\n";
    return s;
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
    //Updating Appearance
    //Hopefully Works, needs testing
    xcom::struct_property& m_kSoldier = static_cast<xcom::struct_property&> (*properties->at(1));
    xcom::struct_property& kAppearance = static_cast<xcom::struct_property&> (*m_kSoldier.properties[10]);
    int i = 0;
    for (const int& app_val : appearance) {
        static_cast<xcom::int_property*> (kAppearance.properties[i].get())->value = app_val;
        i++;
    }
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

        QFile file(QString::fromStdString(path));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("Could not open file: " + path);
        }

        PerkSet perks;
        QTextStream in(&file);
        QString line;
        int i = 0;
        //loads perks from the file in format: index, mobility, aim, will
        //same perk can give different stats for different classes.
        while(!in.atEnd()) {
            line = in.readLine();
            std::vector<std::string> row;
            QStringList tokens = line.split(",");
            for (const QString& token : tokens) {
                row.push_back(token.toStdString());
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

    AppearanceSet appearance(const xcom::property_list* properties) {
        xcom::struct_property& m_kSoldier = static_cast<xcom::struct_property&> (*properties->at(1));
        xcom::struct_property& kAppearance = static_cast<xcom::struct_property&> (*m_kSoldier.properties[10]);
        AppearanceSet appearance;
        for(int i = 0; i < 17; i++) {
            appearance[i] = static_cast<xcom::int_property&> (*kAppearance.properties[i]).value;
        }
        return appearance;
    }

}
