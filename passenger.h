#pragma once
#include "header.h"
#include <memory>
#include <unordered_map>

class bus;

// �q
class passenger
{
private:
	const unsigned int ID;
	Location current_location; // ���ݒn
	std::vector<Location> destinations;
	std::vector<Location>::iterator dest_itr;
	unsigned int waiting_time;

	std::shared_ptr<const bus> riding_bus;

public:
	passenger(unsigned int IDnum, const Graph<Location, unsigned int>& bus_graph, const Location& curr, const Location& dest);

	// true���߂�l�̂Ƃ��͂��łɃS�[���ɒB���Ă���
	bool update(const std::unordered_multimap<Location, const bus*, pair_hash>& buses_at_busstop);

	unsigned int get_waiting_time() const;
};
