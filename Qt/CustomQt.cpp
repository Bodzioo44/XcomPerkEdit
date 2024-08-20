#include "CustomQt.h"

//TODO: switch this shit to csv like file
PerkDisplayMap load_perk_display(PerkSet perks) {
    std::sort(perks.begin(), perks.end(), [](const Perk& a, const Perk& b) { return a.index < b.index; });
    QFile file(":/assets/All_Perk_Data.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("Could not open file: :/assets/All_Perk_Data.txt");
    }

    QTextStream in(&file);
    QString line;
    int current_line = 1;
    unsigned int current_perk = 0;
    bool correct_perk = false;
    PerkDisplay current_PerkDisplay;
    PerkDisplayMap perk_display_map;

    while (!in.atEnd()) {
        line = in.readLine();
        if (current_line % 4 == 1 && line.toInt() == perks[current_perk].index) {
            correct_perk = true;
            current_PerkDisplay.extra_stats = "(Mobility:" + QString::number(perks[current_perk].stats.mobility) + ", Aim:" + QString::number(perks[current_perk].stats.aim) + ", Will:" + QString::number(perks[current_perk].stats.will) + ")";
        } else if (correct_perk) {
            if (current_line % 4 == 2) {
                current_PerkDisplay.name = line;
            } else if (current_line % 4 == 3) {
                current_PerkDisplay.description = line;
            } else if (current_line % 4 == 0) {
                current_PerkDisplay.icon_path = ":/assets/icons/" + line;
                perk_display_map[perks[current_perk].index] = current_PerkDisplay;
                correct_perk = false;
                current_perk++;
                if (current_perk == perks.size()) {
                    break;
                }
            }
        }
        current_line++;
    }
    return perk_display_map;
}

PerkButton::PerkButton(QWidget* parent) : QToolButton(parent) {
    size = 60;
    setIconSize(QSize(size, size));
    QString StyleSheet = "QToolButton { padding: 0px; border: none;}";
    this->setStyleSheet(StyleSheet);
}

void PerkButton::LoadPerk(const PerkDisplay& perk_display) {
    setToolTip("<b>" + perk_display.name + "</b><br>" + perk_display.description + "<br>" + perk_display.extra_stats);
    QIcon unscaled_icon(perk_display.icon_path);
    QPixmap pixmap = unscaled_icon.pixmap(size, size).scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    icon = pixmap;

    QImage img = pixmap.toImage();
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            QColor color = img.pixelColor(x, y);
            int gray = qGray(color.rgb());
            color.setRgb(gray, gray, gray);
            img.setPixelColor(x, y, color);
        }
    }
    greyed_out_icon = QPixmap::fromImage(img);
    is_greyed_out = false;  
    setIcon(icon);
}

void PerkButton::GreyOut() {
    if (!is_greyed_out) {
        setIcon(greyed_out_icon);
        is_greyed_out = true;
    }
}

void PerkButton::LightUp()
{
    if (is_greyed_out) {
        setIcon(icon);
        is_greyed_out = false;
    }
}


bool SoldierTreeItem::operator< (const QTreeWidgetItem &other) const {
    QTreeWidget* tree = treeWidget();
    int sortColumn = tree->sortColumn();
    //class icon compare
    if (sortColumn == 1) {
        QString icon1_path = data(1, Qt::UserRole).toString();
        QString icon2_path = other.data(1, Qt::UserRole).toString();
        return icon1_path < icon2_path;
    }
    //rank icon compare
    if (sortColumn == 2) {
        QString icon1_path = data(2, Qt::UserRole).toString();
        QString icon2_path = other.data(2, Qt::UserRole).toString();
        return icon1_path > icon2_path;
    }
    //edited icon compare
    if (sortColumn == 3) {
        int icon1_path = data(3, Qt::UserRole).toInt();
        int icon2_path = other.data(3, Qt::UserRole).toInt();
        return icon1_path < icon2_path;
    }    
    return QTreeWidgetItem::operator<(other);
    
}