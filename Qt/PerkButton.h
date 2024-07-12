#ifndef PERKBUTTON_H
#define PERKBUTTON_H

#include <QtWidgets/QToolButton>

using perk_data = std::array<std::string, 3>;

class PerkButton : public QToolButton
{
    Q_OBJECT

public:
    PerkButton(QWidget* parent = nullptr);
    ~PerkButton() {};
    void LoadPerk(const perk_data data);
    void GreyedOutSwitch();

private:
    int size;
    bool is_greyed_out;
    QIcon icon;
    QIcon greyed_out_icon;

};

#endif