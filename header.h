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
	bool riding;

public:
	passenger(unsigned int IDnum, const Location& curr, const Location& dest): ID(IDnum), current_location(curr), destination(dest), waiting_time(0), riding(false){}

	bool update_location(const Location& next_location)
	{
		// 1-ノルムで1より多く進んでいたらエラー
        if (ManhattanDistance(current_location, next_location) > 1) { std::cout<< "fuckin' moving " << "(" << current_location.first << "," << current_location.second << ")->(" << next_location.first << "," << next_location.second << ")" << std::endl; return false; }
		current_location = next_location;
		return true;
	}

	bool is_arrived() const { return current_location == destination; }
	bool is_riding() const { return riding; }
	
	void ride() { riding = true; return; }
	void waiting() { waiting_time++; return; }
	unsigned int get_waiting_time() const { return waiting_time; }
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
    bus(unsigned int IDnum, unsigned int cap, const std::vector<Location>& r, unsigned int current_index)
        : ID(IDnum), capacity(cap), current_passengers(), route(r), current_destination_index((current_index + 1) % r.size()), current_location(r.at(current_index)){}
	
	void run()
    {
        // std::cout<< "bus" << this->ID << " was at (" << current_location.first << "," << current_location.second << ")"<< std::endl;
		// バスの移動
		if (current_location.first < route[current_destination_index].first) {
			current_location.first++;
		} else if (current_location.first > route[current_destination_index].first) {
            current_location.first--;
		} else if (current_location.second < route[current_destination_index].second) {
			current_location.second++;
		} else if (current_location.second > route[current_destination_index].second) {
            current_location.second--;
		}

		std::cout << "bus" << ID << " is at (" << current_location.first << "," << current_location.second << ")" << std::endl;

		// 現在の目的地に到着したら
		if (current_location == route[current_destination_index])
		{
			// 目的地を次のバス停に更新
			current_destination_index = (current_destination_index + 1) % route.size();
		}

		// 乗客の状態更新
		for (auto pass_itr = current_passengers.begin(); pass_itr != current_passengers.end(); ) {
			// バスとともに移動
			(*pass_itr)->update_location(current_location);

			// 乗客の現在地と目的地が一致していたら降車させる
			if ((*pass_itr)->get_current_location() == (*pass_itr)->get_destination()) {
				pass_itr = current_passengers.erase(pass_itr);//客のリストから削除
				//std::cout << "someone left from the bus" << std::endl;
				continue;
			}

			// eraseの関係からここでインクリメント
			pass_itr++;
		}
		
		return;
	}
	
	bool is_going_to(const Location& place) {// placeにこのバスが向かうかどうか
		bool flag = false;
		for (unsigned int i = 0; i < route.size(); i++)
		{
			if (route[i] == place) { flag = true; break; }
		}
		return flag;
	}


	bool ride(const std::shared_ptr<passenger>& riding_passenger)
	{
		// capacityを超えないか判定
		if (current_passengers.size() >= capacity) { return false; }

		current_passengers.push_back(riding_passenger);
		riding_passenger->ride();

		return true;
	}

	Location get_current_destination() const { return route[current_destination_index]; }
	Location get_current_location() const { return current_location; }
};

#endif // _INC_CLASS_HEADER
