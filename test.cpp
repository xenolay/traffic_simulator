#include "header.h"
#include <map>
#include <random>
#include <memory>
#include <list>

using std::cout;
using std::endl;

int main(){
	// バス停位置情報
	const unsigned int busstop_num = 4;	// バス停総数
	std::map<unsigned int, std::pair<unsigned int, unsigned int>> busstop_location;
	busstop_location.insert(decltype(busstop_location)::value_type(0, { 1, 1 }));
	busstop_location.insert(decltype(busstop_location)::value_type(1, { 1, 3 }));
	busstop_location.insert(decltype(busstop_location)::value_type(2, { 3, 1 }));
	busstop_location.insert(decltype(busstop_location)::value_type(3, { 3, 3 }));
	// バス停混雑情報
	const unsigned int prob_int[busstop_num] = {100, 50, 5, 100};
	unsigned int cumprob[busstop_num] = { 0 };
	cumprob[0] = prob_int[0];
	for (unsigned int i = 1; i < busstop_num; i++){
		cumprob[i] = cumprob[i - 1] + prob_int[i];
	}
	// 乗客配置
	const unsigned int passenger_num = 100;	// 総乗客数
	std::list<std::shared_ptr<passenger>> passenger_list;
	std::random_device rnd;
	std::uniform_int_distribution<unsigned int> rand(1, cumprob[busstop_num - 1]);
	for (unsigned int i = 0; i < passenger_num; i++)
	{
		// スタートと目的地のバス停をランダムに決定
		unsigned int start_busstop = 0, dest_busstop = 0;
		unsigned int rand_val = rand(rnd);
		for (unsigned int temp = 0; temp < busstop_num; temp++)
		{
			if (rand_val <= cumprob[temp]) { start_busstop = temp; break; }
		}
		rand_val = rand(rnd);
		for (unsigned int temp = 0; temp < busstop_num; temp++)
		{
			if (rand_val <= cumprob[temp]) { dest_busstop = temp; break; }
		}
		if (start_busstop == dest_busstop) { i--; continue; }	// スタートと目的地が一致したらなかったことに
		passenger_list.push_back(std::make_shared<passenger>(start_busstop, dest_busstop));
		auto start_location = busstop_location.at(start_busstop);
		auto dest_location = busstop_location.at(dest_busstop);
		std::cout << i << " : " << start_busstop << "(" << start_location.first << "," << start_location.second << ")" << " -> " << dest_busstop << "(" << dest_location.first << "," << dest_location.second << ")" << std::endl;
	}

    return 0;
}
