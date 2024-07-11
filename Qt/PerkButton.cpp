#include "PerkButton.h"

PerkButton::PerkButton(QWidget* parent, int number) : QToolButton(parent), number(number)
{
    setIconSize(QSize(50, 50));
    QString noPaddingStyleSheet = "QToolButton { padding: 0px; border: none;}";
    this->setStyleSheet(noPaddingStyleSheet);
}


void PerkButton::LoadPerk(const perk_data data)
{
    QString image_name = data[2].c_str();
    QString icon_path = "../assets/icons/" + image_name;
    QString decsription = data[1].c_str();
    QString name = data[0].c_str();

    QIcon icon(icon_path);
    QPixmap pixmap = icon.pixmap(50, 50).scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    setIcon(pixmap);
    //setToolTip(name + "\n" + decsription);
    //setToolTip("<b>" + name + "</b>" + "\n" + decsription);
    setToolTip("<b>" + name + "</b><br>" + decsription);
}

void PerkButton::GreyedOutSwitch()
{
    if (is_greyed_out)
    {
        setStyleSheet("QToolButton {border: none;}");
        is_greyed_out = false;
    }
    else
    {
        setStyleSheet("QToolButton {border: none; background-color: grey;}");
        is_greyed_out = true;
    }
}

// void PerkButton::ChangeIcon(QString icon_path)
// {
//     QIcon icon(icon_path);
//     QSize size = this->size();
//     size.setWidth(50);
//     size.setHeight(50);
//     QPixmap pixmap = icon.pixmap(50, 50).scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
//     QImage image = pixmap.toImage();

//     for (int y = 0; y < image.height(); ++y) {
//         for (int x = 0; x < image.width(); ++x) {
//             QColor color = image.pixelColor(x, y);
//             int h, s, v;
//             color.getHsv(&h, &s, &v);
//             s = qMin(s * 0.01, 255.0);
//             color.setHsv(h, s, v);
//             image.setPixelColor(x, y, color);
//         }
//     }

//     pixmap = QPixmap::fromImage(image);
//     setIcon(pixmap);
// }


// void PerkButton::resizeEvent(QResizeEvent* event)
// {
//     QToolButton::resizeEvent(event);

//     //int min = qMin(width(), height());
//     //setIconSize(QSize(width(), height()));
// }