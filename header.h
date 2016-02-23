#ifndef _INC_CLASS_HEADER
#define _INC_CLASS_HEADER

#include <ios>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <list>

typedef std::pair<unsigned int, unsigned int> Location;
int total_waiting_time;

inline unsigned int ManhattanDistance(const Location& start, const Location& dist)
{
	return static_cast<unsigned int>(std::abs(static_cast<int>(start.first) - static_cast<int>(dist.first)) + std::abs(static_cast<int>(start.second) - static_cast<int>(dist.second)));
}

class passenger { // 客
private:
	const unsigned int ID;
	Location current_location; // 現在地
	const Location destination; // 目的地
	unsigned int waiting_time;

public:
	passenger(unsigned int IDnum, const Location& curr, const Location& dest): ID(IDnum), current_location(curr), destination(dest), waiting_time(0){}

	bool update_location(const Location& next_location)
	{
		// 1-ノルムで1より多く進んでいたらエラー
        if (ManhattanDistance(current_location, next_location) > 1) { std::cout<< "fuckin' moving" << std::endl; return false; }
		current_location = next_location;
		return true;
	}

	bool is_arrived() const { return current_location == destination; }
	
	void wating() { waiting_time++; return; }
	unsigned int get_wating_time() const { return waiting_time; }
	Location get_current_location() const { return current_location; }
	Location get_destination() const { return destination; }
};

class bus{ // バス
private:
	const unsigned int ID;
	const unsigned int capacity; // 定員
	std::list<std::shared_ptr<passenger>> current_passengers;
	std::vector<Location> route;
	unsigned int current_destination_index;
	Location current_location;

public:
	bus(unsigned int IDnum, unsigned int cap, const std::vector<Location>& r, const Location& curr)
		: ID(IDnum), capacity(cap), current_passengers(), route(r), current_destination_index(1), current_location(curr){}
	
	void run()
	{
		current_location = route[current_destination_index];// バスを動かす
        std::list<std::shared_ptr<passenger>>::iterator passitr;//客を指すイテレーター定義
		//バスから下ろす
		for (passitr = current_passengers.begin(); passitr != current_passengers.end(); passitr++) {
            if ((*passitr)->get_current_location() == (*passitr)->get_destination()) {
                total_waiting_time += (*passitr)->get_wating_time();//待ち時間を追加（total_waiting_timeの定義を上に追加？
				current_passengers.remove(*passitr);//客のリストから削除
			}
		}

		current_destination_index++;// current_destination_indexのインクリメント
	}
	
	bool is_going_to(const Location& place) {// placeにこのバスが向かうかどうか
		int i = 0;
		while (i < route.size()) {
			if (route[i] == place) {
				return true;
				break;
			}
			else {
				if (i == route.size()) {
					return false;
					break;
				}
				else {
					i++;
				}
			}
		}
	}


	bool ride(const std::shared_ptr<passenger>& riding_passenger)
	{
		// capacityを超えないか判定
		if (current_passengers.size() >= capacity) { return false; }

		current_passengers.push_back(riding_passenger);
		return true;
	}

	Location get_current_destination() const { return route[current_destination_index]; }
	Location get_current_location() const { return current_location; }
};

#endif // _INC_CLASS_HEADER
