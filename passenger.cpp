#include "passenger.h"
#include "bus.h"

// �o���n�ƖړI�n�A�܂��o�X�̘H���}����A�œK�ȏ�芷���o�H���Z�o(todo)
std::vector<Location> GenTransferRoute(const Graph<Location, unsigned int>& bus_graph, const Location& start, const Location& dest)
{
	// �o�X�̘H����ɂ����ă_�C�N�X�g���@�ɂ��ŒZ�o�H�𓱏o
	return bus_graph.dijkstra(bus_graph.get_vertex_index(start), bus_graph.get_vertex_index(dest));
}

passenger::passenger(unsigned int IDnum, const Graph<Location, unsigned int>& bus_graph, const Location& curr, const Location& dest)
	: ID(IDnum), current_location(curr), destinations(GenTransferRoute(bus_graph, curr, dest)), dest_itr(destinations.begin()), waiting_time(0)
{
	// �o�H�\�z�Ɏ��s������
	if (destinations.size() == 0)
	{
		std::cerr << "Cannot generate transfer route. (bus" << ID << ":" << current_location << "->" << dest << ")" << std::endl;
		return;
	}
}

// true���߂�l�̂Ƃ��͂��łɃS�[���ɒB���Ă���
bool passenger::update(const std::unordered_multimap<Location, const bus*, pair_hash>& buses_at_busstop)
{
	// �o�X��ԍς�
	if (riding_bus)
	{
		// �o�X�ɏ]���Ĉړ�
		if (ManhattanDistance(current_location, riding_bus->get_current_location()) > 1)// 1-�m������1��葽���i��ł�����G���[
		{
			std::cerr << "fuckin' moving " << current_location << "->" << riding_bus->get_current_location() << std::endl;
			return false;
		}
		current_location = riding_bus->get_current_location();

		// �ړ����Č��݂̖ړI�n�Ȃ�~��
		if (current_location == *dest_itr)
		{
			dest_itr++;	// �ړI�n���ړ�
						// �ŏI�ړI�n�������Ȃ�true��Ԃ��ʒm
			if (dest_itr == destinations.end()) { riding_bus = nullptr; return true; }
			// ���̖ړI�n�ɂ͂��̂܂܂ł͍s���Ȃ��Ȃ�~��
			if (!riding_bus->is_going_to(*dest_itr)) { riding_bus = nullptr; }
		}
	}
	else
	{
		// �o�X�ɏ���Ă��Ȃ��Ȃ��낤�Ƃ���
		// ������o�X��Ɏ~�܂��Ă���o�X�𑖍�
		auto itr_range = buses_at_busstop.equal_range(current_location);
		for (auto itr = itr_range.first; itr != itr_range.second; itr++)
		{
			// ���̖ړI�n�ɍs�����`�F�b�N
			if (itr->second->is_going_to(*dest_itr))
			{
				// �s���Ȃ���
				riding_bus = itr->second->ride();
				if (riding_bus) { break; }// ��ԏo������break
			}
		}

		// ���Ȃ�������wait
		if (!riding_bus) { waiting_time++; }
	}

	return false;
}

unsigned int passenger::get_waiting_time() const { return waiting_time; }