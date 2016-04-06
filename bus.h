#pragma once
#include "header.h"
#include <memory>
#include <unordered_map>
#include <QGraphicsItem>

// バス
class bus : public std::enable_shared_from_this<bus>
{
private:
	const unsigned int ID;
	const unsigned int capacity; // 定員
	const std::vector<Location> route;
	Location current_location;
	unsigned int current_destination_index;

public:
	bus(unsigned int IDnum, unsigned int cap, const std::vector<Location>& r, unsigned int current_index);

	void run(std::unordered_multimap<Location, const bus*, pair_hash>* buses_at_busstop);

	// placeにこのバスが向かうかどうか
	bool is_going_to(const Location& place) const;

	std::shared_ptr<const bus> ride() const;

	Location get_current_location() const;
};

class bus_qt : public QGraphicsItem
{
private:
	std::shared_ptr<bus> obj;
	QImage img, current_img;
	QRectF region;
	unsigned int N;

protected:
	void advance(int step) Q_DECL_OVERRIDE;

public:
	bus_qt(const std::shared_ptr<bus>& ptr, const QImage& image, unsigned int gridN, const QRectF& region_rect);

	QRectF boundingRect() const Q_DECL_OVERRIDE;
	//QPainterPath shape() const Q_DECL_OVERRIDE;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
};

