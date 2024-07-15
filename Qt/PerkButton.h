#ifndef PERKBUTTON_H
#define PERKBUTTON_H

#include <QtWidgets/QToolButton>
#include "utils.h"

class PerkButton : public QToolButton
{
    Q_OBJECT

public:
    PerkButton(QWidget* parent = nullptr);
    ~PerkButton() {};
    void LoadPerk(const PerkAssets data, Perk current_perk);
    void GreyOut();
    void LightUp();

private:
    int size;
    bool is_greyed_out;
    QIcon icon;
    QIcon greyed_out_icon;

};

#endif