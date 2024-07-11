#ifndef PERKBUTTON_H
#define PERKBUTTON_H

#include <QtWidgets/QToolButton>
#include <QtCore/QString>

#include <iostream>

#include "utils.h"
using perk_data = std::array<std::string, 3>;

class PerkButton : public QToolButton
{
    Q_OBJECT


public:
    PerkButton(QWidget* parent = nullptr, int number = 0);
    ~PerkButton() {};
    void LoadPerk(const perk_data data);




// signals:
//    void clicked(int i);


private:
    int number;


protected:
    //void resizeEvent(QResizeEvent* event) override;


    
};


#endif