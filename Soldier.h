#ifndef SOLDIER_H
#define SOLDIER_H

#include "xcomsave/xcom.h"
#include <fstream>
#include <sstream>
#include <random>

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QString>

struct SoldierStats {
    int mobility;
    int aim;
    int will;

    SoldierStats(int mobility = 0, int aim = 0, int will = 0): mobility(mobility), aim(aim), will(will) {}

    void operator += (const SoldierStats& stats) {
        mobility = mobility + stats.mobility;
        aim = aim + stats.aim;
        will = will + stats.will;
    }

    void operator -= (const SoldierStats& stats) {
        mobility = mobility - stats.mobility;
        aim = aim - stats.aim;
        will = will - stats.will;
    }
};

struct Perk {
    int index;
    int value;
    SoldierStats stats;
    bool enabled;
    //TODO: confirm this.
    //sometimes perks connected to items have values greater than 1. (f.e. Smoke Grenade, Battle Scanner, Shredder Ammo)
    //from my testing, value represents number of items in the inventory*2 (+ 1 if the perk is enabled)
    //so for example value of 5 means 2 items in the inventory, and the perk is enabled.
    //and value of 2 means 1 item in the inventory, and the perk is disabled.
    Perk(int index = 0, int value = 0, SoldierStats stats = {0, 0, 0}): index(index), value(value), stats(stats) {
        if (value % 2 == 1) {
            enabled = true;
        }
        else {
            enabled = false;
        }
    }
};

using PerkSet = std::array<Perk, 18>;
using AppearanceSet = std::array<int, 17>;
// using LabelSet = std::array<std::string, 3>;

class Soldier {
    public:
        Soldier(xcom::checkpoint* soldier);
        //FIXME: complier is crying about default constructor while 
        //creating map entry in MainWindow::onSoldierSelected().
        //emplace() or insert_or_assign() didnt help either.
        //it was even worse with properties as a reference member.
        //something about stl map pair...
        Soldier() {};
        void EnablePerk(int index);
        void DisablePerk(int index);
        xcom::property_list* GetPropertyList() const;
        QString GetLabels() const;
        PerkSet GetPerks() const;   
        AppearanceSet GetAppearance() const;
        void ApplyAppearancePreset();
        void UpdateSoldier();
        void RevertChanges();

    private:
        xcom::property_list* properties;
        PerkSet perks;
        SoldierStats starting_stats;
        SoldierStats difference_stats;
        AppearanceSet appearance;
        
};

//look into property_visitor again? is there a better way to do it?
namespace GetSoldiers {
    std::string class_type(const xcom::property_list* properties);
    std::string full_name(const xcom::property_list* properties);
    std::string eStatus(const xcom::property_list* properties);
    int rank(const xcom::property_list* properties);
    PerkSet perks(const xcom::property_list* properties);
    SoldierStats stats(const xcom::property_list* properties);
    AppearanceSet appearance(const xcom::property_list* properties);
}

#endif // SOLDIER_H