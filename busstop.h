#pragma once
#include "header.h"
#include <memory>
#include <unordered_map>
#include <QGraphicsItem>

class busstop{};

class busstop_qt : public QGraphicsItem
{
private:
    Location obj;
    QImage img;
    QRectF region;
    unsigned int N;


public:
    busstop_qt(Location& ptr, const QImage& image, unsigned int gridN, const QRectF& region_rect);

    QRectF boundingRect() const Q_DECL_OVERRIDE;
    //QPainterPath shape() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
};

