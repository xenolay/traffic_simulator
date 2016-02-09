#include "header.h"
#include <map>
#include <random>

using std::cout;
using std::endl;

int main(){
	// バス停位置情報
	const unsigned int busstop_num = 4;	// バス停総数
	std::map<unsigned int, Location> busstop_location;
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
		passenger_list.push_back(std::make_shared<passenger>(busstop_location[start_busstop], busstop_location[dest_busstop]));
		auto start_location = busstop_location.at(start_busstop);
		auto dest_location = busstop_location.at(dest_busstop);
		std::cout << i << " : " << start_busstop << "(" << start_location.first << "," << start_location.second << ")" << " -> " << dest_busstop << "(" << dest_location.first << "," << dest_location.second << ")" << std::endl;
	}

	// バスの配置
	std::list<std::shared_ptr<bus>> bus_list;
	//todo

	// メインループ
	unsigned int total_waiting_time = 0;
	while (!passenger_list.empty()) { // 乗客がいるならば	
		// バスを進める & 乗客の降車
		for (auto bus_itr = bus_list.begin(); bus_itr != bus_list.end(); bus_itr++) {
			(*bus_itr)->run();
		}

		//人を乗せる
		for (auto passenger_itr = passenger_list.begin(); passenger_itr != passenger_list.end(); passenger_itr++) {
			for (auto bus_itr = bus_list.begin(); bus_itr != bus_list.end(); bus_itr++) {
				if ((*passenger_itr)->get_current_location() == (*bus_itr)->get_current_location() && (*bus_itr)->is_going_to((*passenger_itr)->get_destination())) {
					(*bus_itr)->ride(*passenger_itr);
				}
			}
		}

		//乗れてない人のカウンタを上げる
		for (auto passenger_itr = passenger_list.begin(); passenger_itr != passenger_list.end(); ) {
			if ((*passenger_itr)->is_arrived()) {	// 目的地に到着したとき
				// 全体の待ち時間を加算
				total_waiting_time += (*passenger_itr)->get_wating_time();
				// 全体の客リストから除外
				passenger_itr = passenger_list.erase(passenger_itr);
				// イテレータの調整
				if (passenger_itr == passenger_list.begin()) { continue; }
				else { passenger_itr--; }
			}
			else if (passenger_itr->use_count() <= 2) {	// バスに乗っていないとき
				(*passenger_itr)->wating();	// 待ち時間を加算
			}

			// continueの関係からここでインクリメント
			passenger_itr++;
		}

		// 全体の待ち時間を出力
		std::cout << total_waiting_time << std::endl;
	}
    return 0;
}
