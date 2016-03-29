#pragma once
#include "header.h"
#include <memory>
#include <vector>
#include <unordered_map>

class bus;
template<typename V, typename W> class Graph;

// 客
class passenger : public std::enable_shared_from_this<passenger>
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

	// nullptrが戻り値のときはすでにゴールに達している
    std::shared_ptr<passenger> update(const std::unordered_multimap<Location, const bus*, pair_hash>& buses_at_busstop);

	bool is_riding() const;
	unsigned int get_waiting_time() const;
    Location get_current_location() const;
};
