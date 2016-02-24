#include "header.h"
#include "traffic_simulator.h"
#include <QApplication>
#include <map>
#include <random>
#include <fstream>
#include <sstream>
#include <algorithm>

using std::cout;
using std::endl;

int main(int argc, char *argv[]){
	// 設定ファイルのロード
    std::ifstream setting_file("C:/Users/k takumi/Desktop/tutorial/data.txt");
    if (setting_file.fail()) { std::cerr << "The setting file wasn't found." << std::endl; return -1; }

	// 全体のサイズ(1辺)
	unsigned int size = 0;
	setting_file >> size;	// 設定ファイルから読み込み

	// バス停情報
	std::map<unsigned int, Location> busstop_location;		// バス停位置情報
	std::vector<unsigned int> busstop_prob;	// バス停混雑情報
	{
		unsigned int busstop_num = 0;	// バス停総数
		setting_file >> busstop_num;	// 設定ファイルから読み込み
        busstop_prob.resize(busstop_num);
		for (unsigned int i = 0; i < busstop_num; i++) {
			unsigned int index = 0, prob = 0, x = 0, y = 0;
			std::string location_str;
			// 設定ファイルから1行読み込み(空白文字区切り)
			setting_file >> index >> location_str >> prob;
			// 座標についてはカンマ区切りになっているので、別に空白で置換の後読み込み
			std::replace(location_str.begin(), location_str.end(), ',', ' ');
			std::stringstream location_stream(location_str);
			location_stream >> x >> y;

			if (x == 0 || y == 0 || x > size || y > size) {
				std::cerr << "The " << index << "-busstop has invalid location, so the busstop wasn't registered." << std::endl;
				continue;
			}
            // 読み込んだ情報の登録
            busstop_location.insert(std::pair<unsigned int, Location>(index, Location(x, y)));
			busstop_prob.at(i) = prob;
		}

		for (unsigned int i = 1; i < busstop_num; i++) { busstop_prob[i] += busstop_prob[i - 1]; }	// 累積確率分布もどきに変換
	}

	// バス路線図
	std::map<unsigned int, std::vector<Location>> bus_route;
	{
		unsigned int bus_route_num = 0;	// 路線図総数
		setting_file >> bus_route_num;	// 設定ファイルから読み込み
		for (unsigned int i = 0; i < bus_route_num; i++) {
			unsigned int index = 0, x = 0, y = 0;
			std::string route_str, tmp;
			// 設定ファイルから1行読み込み(空白文字区切り)
			setting_file >> index >> route_str;
			bus_route.emplace(index, std::vector<Location>());
			// 路線についてはカンマ区切り&バス停index指定なので変換して登録
			std::stringstream route_stream(route_str);
			while(std::getline(route_stream, tmp, ',')){
				bus_route.at(index).push_back(busstop_location.at(std::atoi(tmp.c_str())));
			}
		}
	}

    // バス路線図の出力
    std::cout << "Loaded bus routes." << std::endl;
    for (auto itr = bus_route.begin(); itr != bus_route.end(); itr++)
    {
		std::cout << (*itr).first << " : ";
		std::cout << "(" << (*itr).second.begin()->first << "," << (*itr).second.begin()->second << ")";
        for (auto itr2 = (*itr).second.begin() + 1; itr2 != (*itr).second.end(); itr2++)
		{
			std::cout << "->" << "(" << (*itr2).first << "," << (*itr2).second << ")";
		}
		std::cout << std::endl;
	}

	// 乗客配置
	const unsigned int passenger_num = 100;	// 総乗客数
	std::list<std::shared_ptr<passenger>> passenger_list;
	std::random_device rnd;
	std::uniform_int_distribution<unsigned int> rand(1, busstop_prob[busstop_prob.size() - 1]);
	for (unsigned int i = 0; i < passenger_num; i++)
	{
		// スタートと目的地のバス停をランダムに決定
		unsigned int start_busstop = 0, dest_busstop = 0;
		unsigned int rand_val = rand(rnd);
		for (unsigned int temp = 0; temp < busstop_location.size(); temp++)
		{
			if (rand_val <= busstop_prob[temp]) { start_busstop = temp; break; }
		}
		rand_val = rand(rnd);
		for (unsigned int temp = 0; temp < busstop_location.size(); temp++)
		{
			if (rand_val <= busstop_prob[temp]) { dest_busstop = temp; break; }
		}
		if (start_busstop == dest_busstop) { i--; continue; }	// スタートと目的地が一致したらなかったことに
		// 乗客を全体のリストに登録
		passenger_list.push_back(std::make_shared<passenger>(i, busstop_location[start_busstop], busstop_location[dest_busstop]));
		auto start_location = busstop_location.at(start_busstop);
		auto dest_location = busstop_location.at(dest_busstop);
		std::cout << i << " : " << start_busstop << "(" << start_location.first << "," << start_location.second << ")" << " -> " << dest_busstop << "(" << dest_location.first << "," << dest_location.second << ")" << std::endl;
	}

	// バスの配置
	std::list<std::shared_ptr<bus>> bus_list;
	//todo
    const unsigned int bus_num = 5;	// 総乗客数
    for (unsigned int i = 0; i < bus_num; i++)
    {
        // スタートと目的地のバス停をランダムに決定
        unsigned int start_busstop = 0;
        unsigned int rand_val = rand(rnd);
        for (unsigned int temp = 0; temp < bus_route[0].size(); temp++)
        {
            if (rand_val <= busstop_prob[temp]) { start_busstop = temp; break; }
        }
        rand_val = rand(rnd);
        int dest_busstop=0;
        dest_busstop=rand(rnd)%4;
        // バスを全体のリストに登録
        bus_list.push_back(std::make_shared<bus>(i, 20, bus_route[0],dest_busstop, bus_route[0].at(start_busstop)));
        auto start_location = busstop_location.at(start_busstop);
        auto dest_location=busstop_location.at(dest_busstop);
        std::cout << i << " : " << start_busstop << "(" << start_location.first << "," << start_location.second << ")" <<"->"<<dest_busstop<<"("<<dest_location.first<<","<<dest_location.second<<")"<< std::endl;
    }


	// メインループ
	unsigned int total_waiting_time = 0;
    // while (!passenger_list.empty()) { // 乗客がいるならば
    for (int i=0; i<10; i++){
        bool ride_flag = false;
		// バスを進める & 乗客の降車
		for (auto bus_itr = bus_list.begin(); bus_itr != bus_list.end(); bus_itr++) {
			(*bus_itr)->run();
		}

		//人を乗せる
		for (auto passenger_itr = passenger_list.begin(); passenger_itr != passenger_list.end(); passenger_itr++) {
            // std::cout << "searching for passengers" << std::endl;
			for (auto bus_itr = bus_list.begin(); bus_itr != bus_list.end(); bus_itr++) {
                // std::cout << "searching for buses" << std::endl;
				if ((*passenger_itr)->get_current_location() == (*bus_itr)->get_current_location() && (*bus_itr)->is_going_to((*passenger_itr)->get_destination())) {
                    // std::cout << "Someone rode a bus. Have a good trip." << std::endl;
					(*bus_itr)->ride(*passenger_itr);
                    ride_flag = true;
				}
			}
		}

//        if (!ride_flag){
//            std::cout << "no one rode on any bus. Shit." << std::endl;
//        }

		//乗れてない人のカウンタを上げる
		for (auto passenger_itr = passenger_list.begin(); passenger_itr != passenger_list.end(); ) {
			if (passenger_itr->use_count() <= 2) {	// バスに乗っていないとき
				if ((*passenger_itr)->is_arrived()) {	// 目的地に到着したとき
                    // std::cout << "hooray! someone is arrived!" << std::endl;
					// 全体の待ち時間を加算
					total_waiting_time += (*passenger_itr)->get_wating_time();
					// 全体の客リストから除外
					passenger_itr = passenger_list.erase(passenger_itr);
					// イテレータの調整
					if (passenger_itr == passenger_list.begin()) { continue; }
					else { passenger_itr--; }
				} else {
                    //std::cout<< "fuck! no one is arriving!" << std::endl;
					(*passenger_itr)->wating();	// 待ち時間を加算
				}
			}

			// continueの関係からここでインクリメント
			passenger_itr++;
		}

        // 全体の待ち時間を出力
        std::cout << total_waiting_time << std::endl;
	}
    QApplication a(argc, argv);
    traffic_simulator w;
    w.show();

    return a.exec();
}
