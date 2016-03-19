#pragma once
#include "header.h"
#include <memory>
#include <list>
#include <unordered_map>
#include <QtWidgets>

class passenger;
class bus;

class MainLoop : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(MainLoop)

private:
	std::list<std::shared_ptr<passenger>> passenger_list;
	std::list<std::shared_ptr<bus>> bus_list;
	std::unordered_multimap<Location, const bus*, pair_hash> buses_at_busstop;
	QGraphicsScene scene;
	unsigned int total_waiting_time;

public:
	MainLoop(const std::list<std::shared_ptr<passenger>>& passengers, const std::list<std::shared_ptr<bus>>& buses,
				const std::unordered_multimap<Location, const bus*, pair_hash>& bus_busstop_map, unsigned int gridN);

	QGraphicsScene* get_scene();
	unsigned int get_waiting_time() const;


public slots:
	void run();
};

