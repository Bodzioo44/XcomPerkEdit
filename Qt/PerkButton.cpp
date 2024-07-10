#include "PerkButton.h"

PerkButton::PerkButton(QWidget* parent, int number) : QToolButton(parent), number(number)
{
    setIconSize(QSize(50, 50));
    QString noPaddingStyleSheet = "QToolButton { padding: 0px; }";
    this->setStyleSheet(noPaddingStyleSheet);
    //this->setToolTip(QString::number(number));

}


void PerkButton::ChangeIcon(QString icon_path)
{
    QIcon icon(icon_path);
    QSize size = this->size();
    size.setWidth(50);
    size.setHeight(50);
    QPixmap pixmap = icon.pixmap(50, 50).scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QImage image = pixmap.toImage();

    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QColor color = image.pixelColor(x, y);
            int h, s, v;
            color.getHsv(&h, &s, &v);
            s = qMin(s * 0.01, 255.0);
            color.setHsv(h, s, v);
            image.setPixelColor(x, y, color);
        }
    }

    pixmap = QPixmap::fromImage(image);
    setIcon(pixmap);
}


// void PerkButton::resizeEvent(QResizeEvent* event)
// {
//     QToolButton::resizeEvent(event);

//     //int min = qMin(width(), height());
//     //setIconSize(QSize(width(), height()));
// }