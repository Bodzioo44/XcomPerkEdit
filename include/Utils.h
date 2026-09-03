#ifndef UTILS_H
#define UTILS_H

#include "lib/xcomsave/xcom.h"
#include <QtCore/QString>
#include <QtCore/QFile>

// enum class eStatus {
//     Active,
//     Dead,
//     Healing
// };

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

namespace Extract {
    std::string class_type(const xcom::property_list* properties);
    std::string status(const xcom::property_list* properties);
    int rank(const xcom::property_list* properties);

    QString full_name(const xcom::property_list* properties);
    PerkSet perks(const xcom::property_list* properties);
    SoldierStats stats(const xcom::property_list* properties);
};


#endif // UTILS_H