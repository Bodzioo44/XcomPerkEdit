#include "PerkButton.h"

PerkButton::PerkButton(QWidget* parent) : QToolButton(parent)
{
    size = 60;
    setIconSize(QSize(size, size));
    QString StyleSheet = "QToolButton { padding: 0px; border: none;}";
    this->setStyleSheet(StyleSheet);
}

void PerkButton::LoadPerk(const PerkAssets icon_assets, Perk current_perk)
{
    QString icon_path =  QString::fromStdString("../assets/icons/" + icon_assets.icon_path);
    QString description = icon_assets.description.c_str();
    

    QString name = icon_assets.name.c_str();

    QString extra_stats = "(Mobility:" + QString::number(current_perk.stats.mobility) + ", Aim:" + QString::number(current_perk.stats.aim) + ", Will:" + QString::number(current_perk.stats.will) + ")";

    setToolTip("<b>" + name + "</b><br>" + description + "<br>" + extra_stats);

    QIcon unscaled_icon(icon_path);
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

void PerkButton::GreyOut()
{
    if (!is_greyed_out)
    {
        setIcon(greyed_out_icon);
        is_greyed_out = true;
    }
}

void PerkButton::LightUp()
{
    if (is_greyed_out)
    {
        setIcon(icon);
        is_greyed_out = false;
    }
}
