#ifndef _INC_CLASS_HEADER
#define _INC_CLASS_HEADER

#include <ios>
#include <iostream>
#include <string>
#include <vector>

class edge{ // 重み付きの辺
	int from;
	int to;
	int weight;
};

class bus{ // バス
private:
	int capacity; // 定員
	int current_passengers;
	std::vector<int> route;
public:
	bus(int cap, std::vector<int> r){
		current_passengers = 0;
		capacity = cap;
		route = r;
	}
};

class passenger{ // 客
private:
	int current_location; // 現在地
	int destination; // 目的地
	int waiting_time;
public:
	passenger(int curr, int dest){
		current_location = curr;
		destination = dest;
		waiting_time = 0;
	}
};

#endif // _INC_CLASS_HEADER
