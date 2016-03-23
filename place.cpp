#include "place.h"
#include <QPainter>

place::place(unsigned int size)
    : amount(0) {}

place_qt::place_qt(const std::shared_ptr<place>& ptr, const QImage& image, unsigned int gridN, const QRectF& region_rect)
    : obj(ptr), img(image.scaled(region_rect.width() / gridN, region_rect.height() / gridN, Qt::KeepAspectRatio, Qt::FastTransformation)), region(region_rect), N(gridN) {}

QRectF place_qt::boundingRect() const
{
    return QRectF(img.rect()).adjusted(-0.5, -0.5, 0.5, 0.5);
}

void place_qt::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->drawImage(0, 0, img);
}

void place_qt::advance(int step)
{
    //Location loc = obj->get_current_location();
    //setPos(region.x() + (loc.second - 1) * region.width() / N, region.y() + (loc.first - 1) * region.height() / N);
    //update();
}
