#pragma once
#include "header.h"
#include <memory>
#include <unordered_map>

class bus;

// 客
class passenger
{
private:
	const unsigned int ID;
	Location current_location; // 現在地
	std::vector<Location> destinations;
	std::vector<Location>::iterator dest_itr;
	unsigned int waiting_time;

	std::shared_ptr<const bus> riding_bus;

public:
	passenger(unsigned int IDnum, const Graph<Location, unsigned int>& bus_graph, const Location& curr, const Location& dest);

	// trueが戻り値のときはすでにゴールに達している
	bool update(const std::unordered_multimap<Location, const bus*, pair_hash>& buses_at_busstop);

	unsigned int get_waiting_time() const;
};
