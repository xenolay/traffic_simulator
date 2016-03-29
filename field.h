#pragma once
#include "header.h"
#include <memory>
#include <list>
#include <unordered_map>
#include <QGraphicsItem>

class passenger;

class Field
{
private:
	std::unordered_map<Location, std::list<std::shared_ptr<passenger>>, pair_hash> distribution;
	std::unordered_map<Location, unsigned int, pair_hash> counter;

public:
	Field(const std::list<std::shared_ptr<passenger>>& passenger_list);

	void register_passenger(const std::shared_ptr<passenger>& pass);
	void clear();

	// 指定マスの未乗車乗客数と未乗車乗客待ち時間
	unsigned int get_passenger_num(const Location& loc) const;
	unsigned int get_waiting_time(const Location& loc) const;
	// 指定マスの全乗客数
	unsigned int get_all_passenger_num(const Location& loc) const;
};

class Field_qt : public QGraphicsItem
{
private:
	std::shared_ptr<Field> obj;
	unsigned int N, passenger_num;
	QRectF region;

protected:
	void advance(int step) Q_DECL_OVERRIDE;

public:
	Field_qt(const std::shared_ptr<Field>& ptr, unsigned int gridN, unsigned int all_passenger_num, const QRectF& region_rect);

	QRectF boundingRect() const Q_DECL_OVERRIDE;
	//QPainterPath shape() const Q_DECL_OVERRIDE;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
};
