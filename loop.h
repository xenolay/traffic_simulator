#pragma once
#include "header.h"
#include <memory>
#include <list>
#include <unordered_map>
#include <QtWidgets>

class passenger;
class bus;
class Field;

class CalcLoop
{
private:
	std::list<std::shared_ptr<passenger>> passenger_list;
	std::list<std::shared_ptr<bus>> bus_list;
	std::unordered_multimap<Location, const bus*, pair_hash> buses_at_busstop;
	std::shared_ptr<Field> field;
	QGraphicsScene scene;
	QGraphicsSimpleTextItem *elapsed_text, *waiting_text, *population_text;
	unsigned int total_waiting_time;
	unsigned int elapsed_frame;

	QGraphicsView view;

public:
	CalcLoop(const std::string& title, const std::list<std::shared_ptr<passenger>>& passengers, const std::list<std::shared_ptr<bus>>& buses, std::unordered_map<unsigned int, std::vector<Location>> route,
		const std::unordered_multimap<Location, const bus*, pair_hash>& bus_busstop_map, const std::unordered_map<unsigned int, Location>& busstop_location, unsigned int gridN);

	unsigned int get_waiting_time() const;

	bool run();
	void show();
	void hide();
};

class InitLoop : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(InitLoop)

private:
	unsigned int gridN;
	std::unordered_map<unsigned int, Location> busstop_location;
	std::unordered_map<unsigned int, std::vector<Location>> bus_route;
	std::list<std::shared_ptr<passenger>> passenger_list;

	std::list<std::shared_ptr<bus>> bus_list;
	std::unordered_multimap<Location, const bus*, pair_hash> buses_at_busstop;

	QWidget window;
	QLabel label;
	QSpinBox* spinboxes;
	QPushButton button;

	unsigned int bus_num;
	unsigned int bus_capacity;
	bool input_end, init_end;

private slots:
	void confirmed();
	void check(int);

public:
	InitLoop(const std::string& title, const std::string& filename, unsigned int passenger_num, unsigned int bus_num, unsigned int bus_capacity);
	~InitLoop();

	bool run();
	void show();
	void hide();

	std::shared_ptr<CalcLoop> genCalcLoop() const;
};


class MainLoop : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(MainLoop)

private:
	std::shared_ptr<InitLoop> init_loop;
	std::shared_ptr<CalcLoop> calc_loop;

	enum class LoopState
	{
		INIT,
		CALC,
		END
	};

	LoopState state;
	unsigned int result;

public:
	MainLoop(const std::string& title, const std::string& filename, unsigned int passenger_num, unsigned int bus_num, unsigned int bus_capacity);
	unsigned int get_result() const;

public slots:
	void run();
};
