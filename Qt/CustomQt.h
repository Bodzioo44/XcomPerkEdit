#ifndef CUSTOMQT_H
#define CUSTOMQT_H

#include <QtWidgets/QToolButton>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QApplication>
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

class SoldierTreeItem : public QTreeWidgetItem {

public:
    bool operator< (const QTreeWidgetItem &other) const;
};

class CustomWidget : public QWidget {
    Q_OBJECT

public:
    template<typename... Args>
    CustomWidget(Args... args) {
        layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        addWidgetsToLayout(args...);
        setLayout(layout);
    }
    ~CustomWidget() {
        delete layout;
    };
private:
    void addWidgetToLayout(const QIcon& icon) {
        QLabel* label = new QLabel(this);
        label->setPixmap(icon.pixmap(20, 20));
        layout->addWidget(label);
    }
    void addWidgetToLayout(const QString& text) {
        QLabel* label = new QLabel(this);
        label->setText(text);
        layout->addWidget(label);
    }

    template<typename... Args>
    void addWidgetsToLayout(Args... args) {
        (addWidgetToLayout(args), ...);
    }
    QHBoxLayout* layout;

};

#endif