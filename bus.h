#pragma once
#include "header.h"
#include <memory>
#include <unordered_map>

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
