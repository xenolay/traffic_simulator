#ifndef _INC_CLASS_HEADER
#define _INC_CLASS_HEADER

#include <ios>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <list>
#include <unordered_map>

typedef std::pair<unsigned int, unsigned int> Location;

template<class T1, class T2>
std::ostream& operator<<(std::ostream& s, const std::pair<T1, T2>& p)
{
	return s << "(" << p.first << "," << p.second << ")";
}

template<typename T> void hash_combine(size_t& seed, T const& v){ seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2); }

struct pair_hash
{
	template<class T1, class T2> std::size_t operator()(const std::pair<T1, T2>& p) const
	{
		std::size_t seed = 0;
		hash_combine(seed, p.first);
		hash_combine(seed, p.second);
		return seed;
	}
};

inline unsigned int ManhattanDistance(const Location& start, const Location& dist)
{
	return static_cast<unsigned int>(std::abs(static_cast<int>(start.first) - static_cast<int>(dist.first)) + std::abs(static_cast<int>(start.second) - static_cast<int>(dist.second)));
}

// 出発地と目的地、またバスの路線図から、最適な乗り換え経路を算出(todo)
std::vector<Location> GenTransferRoute(const Location& start, const Location& dest, const std::unordered_map<unsigned int, std::vector<Location>>& routes)
{
	return std::vector<Location>(1, dest);
}

// バス
class bus : public std::enable_shared_from_this<bus>
{
private:
	const unsigned int ID;
	const unsigned int capacity; // 定員
	const std::vector<Location> route;
	unsigned int current_passenger_num;//temp
	Location current_location;
	unsigned int current_destination_index;

public:
	bus(unsigned int IDnum, unsigned int cap, const std::vector<Location>& r, unsigned int current_index)
		: ID(IDnum), capacity(cap), route(r), current_passenger_num(0), current_location(r.at(current_index)), current_destination_index((current_index + 1) % r.size()) {}

	void run(std::unordered_multimap<Location, const bus*, pair_hash>* buses_at_busstop)
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

		std::cout << "bus" << ID << " is at " << current_location << std::endl;

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
	bool is_going_to(const Location& place) const
	{
		for (unsigned int i = 0; i < route.size(); i++){ if (route[i] == place) { return true; } }
		return false;
	}

	std::shared_ptr<const bus> ride() const
	{
		// capacityを超えないか判定
		if (shared_from_this().use_count() - 2 >= capacity) { return nullptr; }
		return shared_from_this();
	}

	Location get_current_location() const { return current_location; }
};

class passenger { // 客
private:
	const unsigned int ID;
	Location current_location; // 現在地
	std::vector<Location> destinations;
	std::vector<Location>::iterator dest_itr;
	unsigned int waiting_time;

	std::shared_ptr<const bus> riding_bus;

public:
	passenger(unsigned int IDnum, const Location& curr, const Location& dest, const std::unordered_map<unsigned int, std::vector<Location>>& routes)
		: ID(IDnum), current_location(curr), destinations(GenTransferRoute(curr,dest,routes)), dest_itr(destinations.begin()), waiting_time(0)
	{
		// 経路構築に失敗したら
		if (destinations.size() == 0)
		{
			std::cerr << "Cannot generate transfer route. (bus" << ID << ":" << current_location << "->" << dest << ")" << std::endl;
			return;
		}
	}

	// trueが戻り値のときはすでにゴールに達している
	bool update(const std::unordered_multimap<Location, const bus*, pair_hash>& buses_at_busstop)
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
				riding_bus = nullptr;
				dest_itr++;	// 目的地を移動
				// 最終目的地だったならtrueを返し通知
				if (dest_itr == destinations.end()) { return true; }
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
				if(itr->second->is_going_to(*dest_itr))
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

	unsigned int get_waiting_time() const { return waiting_time; }
};

#endif // _INC_CLASS_HEADER
