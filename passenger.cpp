#include "passenger.h"
#include "bus.h"
#include "place.h"
#include "graph.h"

// 出発地と目的地、またバスの路線図から、最適な乗り換え経路を算出(todo)
std::vector<Location> GenTransferRoute(const Graph<Location, unsigned int>& bus_graph, const Location& start, const Location& dest)
{
	// バスの路線上においてダイクストラ法により最短経路を導出
	return bus_graph.dijkstra(bus_graph.get_vertex_index(start), bus_graph.get_vertex_index(dest));
}

passenger::passenger(unsigned int IDnum, const Graph<Location, unsigned int>& bus_graph, const Location& curr, const Location& dest)
	: ID(IDnum), current_location(curr), destinations(GenTransferRoute(bus_graph, curr, dest)), dest_itr(destinations.begin()), waiting_time(0)
{
	// 経路構築に失敗したら
	if (destinations.size() == 0)
	{
		std::cerr << "Cannot generate transfer route. (bus" << ID << ":" << current_location << "->" << dest << ")" << std::endl;
		return;
	}
}

// trueが戻り値のときはすでにゴールに達している
bool passenger::update(const std::unordered_multimap<Location, const bus*, pair_hash>& buses_at_busstop)
{
	// バス乗車済み
	if (riding_bus)
    {
        // バスに従って移動
		if (ManhattanDistance(current_location, riding_bus->get_current_location()) > 1)// 1-ノルムで1より多く進んでいたらエラー
		{
			std::cerr << "fuckin' moving " << current_location << "->" << riding_bus->get_current_location() << std::endl;
			return false;
		}
		current_location = riding_bus->get_current_location();

		// 移動して現在の目的地なら降車
		if (current_location == *dest_itr)
		{
			dest_itr++;	// 目的地を移動
						// 最終目的地だったならtrueを返し通知
            if (dest_itr == destinations.end()) { riding_bus = nullptr; return true; }
			// 次の目的地にはこのままでは行けないなら降車
			if (!riding_bus->is_going_to(*dest_itr)) { riding_bus = nullptr; }
		}
	}
	else
	{
		// バスに乗っていないなら乗ろうとする
		// 今いるバス停に止まっているバスを走査
		auto itr_range = buses_at_busstop.equal_range(current_location);
		for (auto itr = itr_range.first; itr != itr_range.second; itr++)
		{
			// 次の目的地に行くかチェック
			if (itr->second->is_going_to(*dest_itr))
			{
				// 行くなら乗車
				riding_bus = itr->second->ride();
				if (riding_bus) { break; }// 乗車出来たらbreak
			}
		}

		// 乗れなかったらwait
		if (!riding_bus) { waiting_time++; }
	}

	return false;
}

unsigned int passenger::get_waiting_time() const { return waiting_time; }

Location passenger::get_current_location() { return current_location; }
