#include "field.h"
#include "passenger.h"
#include <QPainter>

Field::Field(const std::list<std::shared_ptr<passenger>>& passenger_list) : distribution(), counter()
{
	for (auto itr : passenger_list)
	{
		if (!itr->is_riding()) { distribution[itr->get_current_location()].push_back(itr); }
		counter[itr->get_current_location()]++;
	}
	return;
}

void Field::register_passenger(const std::shared_ptr<passenger>& pass)
{
	if (!pass->is_riding()) { distribution[pass->get_current_location()].push_back(pass); }
	counter[pass->get_current_location()]++;
	return;
}

void Field::clear() { distribution.clear(); counter.clear(); }

unsigned int Field::get_passenger_num(const Location& loc) const
{
	auto itr = distribution.find(loc);
	if (itr == distribution.end()) { return 0; }
	return itr->second.size();
}

unsigned int Field::get_waiting_time(const Location& loc) const
{
	if (distribution.find(loc) == distribution.end()) { return 0; }

	unsigned int waiting_time = 0;
	for (auto itr : distribution.at(loc)){ waiting_time += itr->get_waiting_time(); }
	return waiting_time;
}

unsigned int Field::get_all_passenger_num(const Location& loc) const
{
	auto itr = counter.find(loc);
	if (itr == counter.end()) { return 0; }
	return itr->second;
}

Field_qt::Field_qt(const std::shared_ptr<Field>& ptr, unsigned int gridN, unsigned int all_passenger_num, const QRectF& region_rect)
	: obj(ptr), N(gridN), passenger_num(all_passenger_num), region(region_rect){}

void Field_qt::advance(int step)
{
	if (step == 0)
	{
		// どこで何人待っているかのを表示(括弧内はマス目上に存在している人数)
		for (unsigned int i = 1; i < N + 1; i++)
		{
			for (unsigned int j = 1; j < N + 1; j++) {
				Location loc(i, j);
				std::cout << loc << ": waiting " << obj->get_passenger_num(loc) << " (" << obj->get_all_passenger_num(loc) << ")" << std::endl;
			}
		}
	}
	return;
}

QRectF Field_qt::boundingRect() const { return region; }

void Field_qt::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	// 各マス目をマス目上の未乗車乗客の人数を透明度に、総待ち時間を色(色相)として表示
	for (unsigned int i = 1; i < N + 1; i++)
	{
		for (unsigned int j = 1; j < N + 1; j++) {
			Location loc(i, j);
			QRectF paint_region(region.x() + (loc.second - 1) * region.width() / N, region.y() + (loc.first - 1) * region.height() / N, region.width() / N, region.height() / N);
			QColor color(QColor::fromHsvF(std::max(0, static_cast<int>(240 - obj->get_waiting_time(loc))) / 360.0, 1.0, 1.0, obj->get_passenger_num(loc) / static_cast<double>(passenger_num)));
			painter->fillRect(paint_region, color);
		}
	}
	return;
}
