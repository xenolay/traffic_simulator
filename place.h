#pragma once
#include "header.h"
#include <memory>
#include <unordered_map>
#include <QGraphicsItem>

// 場所管理
class place : public std::enable_shared_from_this<place>
{
private:

public:
    unsigned int amount; // そこにいる人間の数
    place(unsigned int size);

    Location get_location() const;
};

class place_qt : public QGraphicsItem
{
private:
    std::shared_ptr<place> obj;
    QImage img;
    QRectF region;
    unsigned int N;

protected:
    void advance(int step) Q_DECL_OVERRIDE;

public:
    place_qt(const std::shared_ptr<place>& ptr, const QImage& image, unsigned int gridN, const QRectF& region_rect);

    QRectF boundingRect() const Q_DECL_OVERRIDE;
    //QPainterPath shape() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
};

