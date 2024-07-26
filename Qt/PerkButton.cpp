#include "PerkButton.h"

//TODO: switch this shit to csv like file
PerkDisplayMap load_perk_display(PerkSet perks) {
    std::sort(perks.begin(), perks.end(), [](const Perk& a, const Perk& b) { return a.index < b.index; });
    std::ifstream file("../assets/All_Perk_Data.txt");
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: ../assets/All_Perk_Data.txt");
    }
    std::string line;
    int current_line = 1;
    unsigned int current_perk = 0;
    bool correct_perk = false;
    PerkDisplay current_PerkDisplay;
    PerkDisplayMap perk_display_map;
    while (std::getline(file, line)) {
        if (current_line % 4 == 1 && std::stoi(line) == perks[current_perk].index) {
            correct_perk = true;
            current_PerkDisplay.extra_stats = "(Mobility:" + QString::number(perks[current_perk].stats.mobility) + ", Aim:" + QString::number(perks[current_perk].stats.aim) + ", Will:" + QString::number(perks[current_perk].stats.will) + ")";
        }
        else if (correct_perk) {
            if (current_line % 4 == 2) {
                current_PerkDisplay.name = QString::fromStdString(line);
            }
            else if (current_line % 4 == 3) {
                current_PerkDisplay.description = QString::fromStdString(line);
            }
            else if (current_line % 4 == 0) {
                current_PerkDisplay.icon_path = QString::fromStdString("../assets/icons/" + line);
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
    //TODO: try to move this inside .ui file
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
