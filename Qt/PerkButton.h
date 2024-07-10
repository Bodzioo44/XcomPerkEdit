#ifndef PERKBUTTON_H
#define PERKBUTTON_H

#include <QtWidgets/QToolButton>

#include <iostream>


class PerkButton : public QToolButton
{
    Q_OBJECT


public:
    PerkButton(QWidget* parent = nullptr, int number = 0);
    ~PerkButton() {};
    void ChangeIcon(QString icon_path);
    



// signals:
//    void clicked(int i);


private:
    int number;


protected:
    //void resizeEvent(QResizeEvent* event) override;


    
};


#endif