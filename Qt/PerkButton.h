#ifndef PERKBUTTON_H
#define PERKBUTTON_H

#include <QtWidgets/QToolButton>
#include <Soldier.h>

struct PerkDisplay {
    QString name;
    QString description;
    QString icon_path;
    QString extra_stats;
};

using PerkDisplayMap = std::map<int, PerkDisplay>;
PerkDisplayMap load_perk_display(PerkSet perks);

class PerkButton : public QToolButton {
    Q_OBJECT

public:
    PerkButton(QWidget* parent = nullptr);
    ~PerkButton() {};
    void LoadPerk(const PerkDisplay& perk_assets);
    void GreyOut();
    void LightUp();

private:
    int size;
    bool is_greyed_out;
    QIcon icon;
    QIcon greyed_out_icon;
};

#endif