#include "bus.h"
#include <QPainter>

bus::bus(unsigned int IDnum, unsigned int cap, const std::vector<Location>& r, unsigned int current_index)
	: ID(IDnum), capacity(cap), route(r), current_location(r.at(current_index)), current_destination_index((current_index + 1) % r.size()) {}

void bus::run(std::unordered_multimap<Location, const bus*, pair_hash>* buses_at_busstop)
{
	// バスの移動
	if (current_location.first < route[current_destination_index].first) {
		current_location.first++;
	}
	else if (current_location.first > route[current_destination_index].first) {
		current_location.first--;
	}
	else if (current_location.second < route[current_destination_index].second) {
		current_location.second++;
	}
	else if (current_location.second > route[current_destination_index].second) {
		current_location.second--;
	}

    //std::cout << "bus" << ID << " is at " << current_location << std::endl;

	// 現在の目的地に到着したら
	if (current_location == route[current_destination_index])
	{
		// 目的地を次のバス停に更新
		current_destination_index = (current_destination_index + 1) % route.size();
		// バス停にいるはずなので、登録
		buses_at_busstop->insert(std::make_pair(current_location, this));
	}

	return;
}

// placeにこのバスが向かうかどうか
bool bus::is_going_to(const Location& place) const
{
	for (unsigned int i = 0; i < route.size(); i++) { if (route[i] == place) { return true; } }
	return false;
}

std::shared_ptr<const bus> bus::ride() const
{
	// capacityを超えないか判定
	if (shared_from_this().use_count() - 2 >= capacity) { return nullptr; }
	return shared_from_this();
}

Location bus::get_current_location() const { return current_location; }


bus_qt::bus_qt(const std::shared_ptr<bus>& ptr, const QImage& image, unsigned int gridN, const QRectF& region_rect)
	: obj(ptr), img(image), current_img(img), region(region_rect), N(gridN)
{
	setTransformOriginPoint(img.rect().center());
}

QRectF bus_qt::boundingRect() const
{
	return QRectF(current_img.rect()).adjusted(-0.5, -0.5, 0.5, 0.5);
}

void bus_qt::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	painter->drawImage(0, 0, current_img);
}

void bus_qt::advance(int step)
{
	if (step == 0)
	{
		Location loc = obj->get_current_location();
		auto new_x = region.x() + (loc.second - 1) * img.rect().width();
		auto new_y = region.y() + (loc.first - 1) * img.rect().height();
		if (new_x >= pos().x()) { current_img = img.mirrored(false, false); }
		else if (new_x < pos().x()) { current_img = img.mirrored(true, false); }
		if (new_y > pos().y()) { setRotation(90); }
		else if (new_y < pos().y()) { setRotation(-90); }
		else { setRotation(0); }
		setPos(new_x, new_y);
		update();
	}
	return;
}
