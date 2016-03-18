#include "bus.h"

bus::bus(unsigned int IDnum, unsigned int cap, const std::vector<Location>& r, unsigned int current_index)
	: ID(IDnum), capacity(cap), route(r), current_location(r.at(current_index)), current_destination_index((current_index + 1) % r.size()) {}

void bus::run(std::unordered_multimap<Location, const bus*, pair_hash>* buses_at_busstop)
{
	// �o�X�̈ړ�
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

	std::cout << "bus" << ID << " is at " << current_location << std::endl;

	// ���݂̖ړI�n�ɓ���������
	if (current_location == route[current_destination_index])
	{
		// �ړI�n�����̃o�X��ɍX�V
		current_destination_index = (current_destination_index + 1) % route.size();
		// �o�X��ɂ���͂��Ȃ̂ŁA�o�^
		buses_at_busstop->insert(std::make_pair(current_location, this));
	}

	return;
}

// place�ɂ��̃o�X�����������ǂ���
bool bus::is_going_to(const Location& place) const
{
	for (unsigned int i = 0; i < route.size(); i++) { if (route[i] == place) { return true; } }
	return false;
}

std::shared_ptr<const bus> bus::ride() const
{
	// capacity�𒴂��Ȃ�������
	if (shared_from_this().use_count() - 2 >= capacity) { return nullptr; }
	return shared_from_this();
}

Location bus::get_current_location() const { return current_location; }
