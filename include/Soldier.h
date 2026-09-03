#ifndef SOLDIER_H
#define SOLDIER_H

#include "lib/xcomsave/xcom.h"
#include <fstream>
#include <sstream>
#include <random>


#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QString>
#include <QtCore/QDebug>

struct SoldierStats {
    int mobility;
    int aim;
    int will;
    int fatigue;
    int meld;

    SoldierStats(int mobility = 0, int aim = 0, int will = 0, int fatigue = 0, int meld = 0): mobility(mobility), aim(aim), will(will), fatigue(fatigue), meld(meld) {}

    void operator += (const SoldierStats& stats) {
        mobility = mobility + stats.mobility;
        aim = aim + stats.aim;
        will = will + stats.will;
        fatigue = fatigue + stats.fatigue;
        meld = meld + stats.meld;
    }

    void operator -= (const SoldierStats& stats) {
        mobility = mobility - stats.mobility;
        aim = aim - stats.aim;
        will = will - stats.will;
        fatigue = fatigue - stats.fatigue;
        meld = meld - stats.meld;
    }
};

// enum class PerkType : int {
//     Standard = 0,
//     Gene = 1,
//     PSI = 2
// };

struct Perk {
    int index;
    int order;
    int value;
    bool enabled;
    // enum PerkType type;
    //TODO: confirm this.
    //sometimes perks connected to items have values greater than 1. (f.e. Smoke Grenade, Battle Scanner, Shredder Ammo)
    //from my testing, value represents number of items in the inventory*2 (+ 1 if the perk is enabled)
    //so for example value of 5 means 2 items in the inventory, and the perk is enabled.
    //and value of 2 means 1 item in the inventory, and the perk is disabled.
    Perk(int index = 0, int order = 0, int value = 0): index(index), order(order), value(value) {
        if (value % 2 == 1) {
            enabled = true;
        }
        else {
            enabled = false;
        }
    }

    virtual QString GetDescription() = 0;
    virtual void UpdateDiff(SoldierStats &Diff) = 0;
};

struct StandardPerk : Perk {
    SoldierStats stats;
    StandardPerk(int index, int order, int value, SoldierStats stats): Perk(index, order, value), stats(stats) {}

    QString GetDescription() override {
        QString description = "Mobility: " + QString::number(stats.mobility) + ", Aim: " + QString::number(stats.aim) + ", Will: " + QString::number(stats.will);
        return description;
    }

    void UpdateDiff(SoldierStats &Diff) override {
        Diff.mobility += stats.mobility;
        Diff.aim += stats.aim;
        Diff.will += stats.will;
    }
};

struct PSIPerk : Perk {
    int fatigue;
    PSIPerk(int index, int order, int value, int fatigue): Perk(index, order, value), fatigue(fatigue) {}

    QString GetDescription() override {
        QString description = "Fatigue: " + QString::number(fatigue);
        return description;
    }

    void UpdateDiff(SoldierStats &Diff) override {
        Diff.fatigue += fatigue;
    }
};

struct GenePerk : Perk {
    int fatigue;
    int meld;
    GenePerk(int index, int order, int value, int fatigue, int meld): Perk(index, order, value), fatigue(fatigue), meld(meld) {}

    QString GetDescription() override {
        QString description = "Fatigue: " + QString::number(fatigue) + ", Meld: " + QString::number(meld);
        return description;
    }

    void UpdateDiff(SoldierStats &Diff) override {
        Diff.fatigue += fatigue;
        Diff.meld += meld;
    }
};

using PerkSet = std::array<Perk, 30>;


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
        // AppearanceSet GetAppearance() const;
        // void ApplyAppearancePreset(AppearanceSet preset);
        void UpdateSoldier();
        void RevertChanges();

    private:
        xcom::property_list* properties;
        PerkSet perks;
        SoldierStats starting_stats;
        SoldierStats difference_stats;

        // AppearanceSet appearance;
        
};

//look into property_visitor again? is there a better way to do it?
namespace GetSoldiers {
    std::string class_type(const xcom::property_list* properties);
    std::string full_name(const xcom::property_list* properties);
    std::string eStatus(const xcom::property_list* properties);
    int rank(const xcom::property_list* properties);
    PerkSet perks(const xcom::property_list* properties);
    SoldierStats stats(const xcom::property_list* properties);
    // AppearanceSet appearance(const xcom::property_list* properties);
}

#endif // SOLDIER_H