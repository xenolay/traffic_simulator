#include "busstop.h"
#include <QPainter>

busstop_qt::busstop_qt(Location &ptr, const QImage& image, unsigned int gridN, const QRectF& region_rect)
    : obj(ptr), img(image.scaled(region_rect.width() / gridN, region_rect.height() / gridN, Qt::KeepAspectRatio, Qt::FastTransformation)), region(region_rect), N(gridN)
{}

QRectF busstop_qt::boundingRect() const
{
    return QRectF(img.rect()).adjusted(-0.5, -0.5, 0.5, 0.5);
}

void busstop_qt::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->drawImage(0, 0, img);
    Location loc = obj;
    setPos(region.x() + (loc.second - 1) * region.width() / N, region.y() + (loc.first - 1) * region.height() / N);
    update();
}

