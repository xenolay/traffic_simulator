#include "header.h"
#include "traffic_simulator.h"
#include <QtWidgets>
#include <math.h>
#include <QApplication>
#include <random>
#include <fstream>
#include <sstream>
#include <algorithm>

using std::cout;
using std::endl;

int main(int argc, char *argv[]){
	// 設定ファイルのロード
    std::ifstream setting_file("../traffic_simulator/data.txt");
    if (setting_file.fail()) { std::cerr << "The setting file wasn't found." << std::endl; return -1; }

	// 全体のサイズ(1辺)
	unsigned int size = 0;
	setting_file >> size;	// 設定ファイルから読み込み

	// バス停情報
	std::unordered_map<unsigned int, Location> busstop_location;		// バス停位置情報
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
	std::unordered_map<unsigned int, std::vector<Location>> bus_route;
	{
		unsigned int bus_route_num = 0;	// 路線図総数
		setting_file >> bus_route_num;	// 設定ファイルから読み込み
		std::cout << "Bus Routes:" << std::endl;
		for (unsigned int i = 0; i < bus_route_num; i++) {
			unsigned int index = 0;
			std::string route_str, tmp;
			// 設定ファイルから1行読み込み(空白文字区切り)
			setting_file >> index >> route_str;
			bus_route.emplace(index, std::vector<Location>());
			// 路線についてはカンマ区切り&バス停index指定なので変換して登録
			std::stringstream route_stream(route_str);
			while (std::getline(route_stream, tmp, ',')) {
				bus_route.at(index).push_back(busstop_location.at(std::atoi(tmp.c_str())));
			}
			// バス路線図の出力
			std::cout << index << " : " << bus_route.at(index).front();
			for (auto itr = bus_route.at(index).begin() + 1; itr != bus_route.at(index).end(); itr++)
			{
				std::cout << "->" << *itr;
			}
			std::cout << std::endl;
		}		
	}

	// 乗り換え経路用にグラフの作成
	std::vector<Location> bus_stops;
	{
		for (unsigned int i = 0; i < bus_route.size(); i++) { bus_stops.insert(bus_stops.end(), bus_route.at(i).begin(), bus_route.at(i).end()); }
		std::sort(bus_stops.begin(), bus_stops.end());
		bus_stops.erase(std::unique(bus_stops.begin(), bus_stops.end()), bus_stops.end());
	}

	std::vector<std::pair<unsigned int, unsigned int>> all_bus_routes;
	{
		for (unsigned int i = 0; i < bus_route.size(); i++)
		{
			auto s = std::distance(bus_stops.begin(), std::find(bus_stops.begin(), bus_stops.end(), bus_route.at(i).at(0)));
			for (unsigned int j = 0; j < bus_route.at(i).size() - 1; j++)
			{
				auto e = std::distance(bus_stops.begin(), std::find(bus_stops.begin(), bus_stops.end(), bus_route.at(i).at(j + 1)));
				all_bus_routes.push_back(std::make_pair(static_cast<unsigned int>(s), static_cast<unsigned int>(e)));
				s = e;
			}
		}
		std::sort(all_bus_routes.begin(), all_bus_routes.end());
		all_bus_routes.erase(std::unique(all_bus_routes.begin(), all_bus_routes.end()), all_bus_routes.end());
	}
	Graph<Location, unsigned int> bus_graph(bus_stops, all_bus_routes, ManhattanDistance);

	// 乗客配置
	const unsigned int passenger_num = 100;	// 総乗客数
	std::list<std::shared_ptr<passenger>> passenger_list;
	{
		std::random_device rnd;
		std::uniform_int_distribution<unsigned int> rand(1, busstop_prob[busstop_prob.size() - 1]);
		std::cout << "Passenger Distribution:" << std::endl;
		for (unsigned int i = 0; i < passenger_num; i++)
		{
			// スタートと目的地のバス停をランダムに決定(バス停ごとに定められた確率に応じて)
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
			passenger_list.push_back(std::make_shared<passenger>(i, bus_graph, busstop_location.at(start_busstop), busstop_location.at(dest_busstop)));
			// 登録した乗客のスタート地点とゴール地点を表示
			std::cout << i << " : " << start_busstop << busstop_location.at(start_busstop) << " -> " << dest_busstop << busstop_location.at(dest_busstop) << std::endl;
		}
	}
	
	// バス配置
	const unsigned int bus_num = 5;	// バス総台数
	const unsigned int bus_capacity = 20; // バス定員
	std::list<std::shared_ptr<bus>> bus_list;
	std::unordered_multimap<Location, const bus*, pair_hash> buses_at_busstop;
	{
		// バス路線の配分の入力
		std::vector<unsigned int> bus_allocation(bus_route.size());
		{
			bool valid_input = false;
			while (!valid_input)
			{
				std::cout << "Allocate " << bus_num << " buses among the " << bus_route.size() << " bus routes with space sparated format." << std::endl;
				unsigned int sum = 0;
				for (unsigned int i = 0; i < bus_allocation.size(); i++) { std::cin >> bus_allocation[i]; sum += bus_allocation[i]; }
				if (sum == bus_num) { valid_input = true; }
				else
				{
					std::cout << "The sum of allocated buses doesn't match the number of all buses." << std::endl;
				}
			}
		}

		// バス配置
		std::random_device rnd;
		std::cout << "Bus Distribution:" << std::endl;
		for (unsigned int i = 0; i < bus_allocation.size(); i++)
		{
			std::cout << "<Route" << i << ">" << std::endl;
			std::uniform_int_distribution<unsigned int> rand(0, static_cast<unsigned int>(bus_route[i].size() - 1));
			for (unsigned int j = 0; j < bus_allocation.at(i); j++)
			{
				// スタートのバス停をランダムに決定
				unsigned int start_busstop = rand(rnd);
				// バスを全体のリストに登録
				bus_list.push_back(std::make_shared<bus>(static_cast<unsigned int>(bus_list.size()), bus_capacity, bus_route[i], start_busstop));
				buses_at_busstop.insert(std::make_pair(bus_list.back()->get_current_location(), bus_list.back().get()));
				// 登録したバスの初期位置表示
				std::cout << bus_list.size() - 1 << " : " << start_busstop << bus_list.back()->get_current_location() << std::endl;
			}
		}
	}

	    // 描画に必要なWindowの用意
    QApplication app(argc, argv);
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));

    QGraphicsScene scene;
    scene.setSceneRect(-300, -300, 600, 600);
    scene.setItemIndexMethod(QGraphicsScene::NoIndex);

    QGraphicsView view(&scene);
    view.setRenderHint(QPainter::Antialiasing);
    view.setBackgroundBrush(QPixmap(":/images/cheese.jpg"));
    view.setCacheMode(QGraphicsView::CacheBackground);
    view.setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    view.setDragMode(QGraphicsView::ScrollHandDrag);
    view.setWindowTitle(QT_TRANSLATE_NOOP(QGraphicsView, "Traffic Simulator"));
    view.resize(1200, 800);
    view.show();

    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), &scene, SLOT(advance()));
    timer.start(1000 / 33);

	// メインループ
	std::cout << "Calculation:" << std::endl;
	unsigned int total_waiting_time = 0;
	// 乗客がいる限り処理
	while (!passenger_list.empty()) {
		// 全乗客の更新処理
		for (auto passenger_itr = passenger_list.begin(); passenger_itr != passenger_list.end(); ) {
			if ((*passenger_itr)->update(buses_at_busstop))
			{
				// 目的地に到着した場合
				// 全体の待ち時間を加算
				total_waiting_time += (*passenger_itr)->get_waiting_time();
				// 全体の客リストから除外
				passenger_itr = passenger_list.erase(passenger_itr);

				continue;
			}
			passenger_itr++;
		}

		buses_at_busstop.clear();

		// バスを進める
		for (auto bus_itr = bus_list.begin(); bus_itr != bus_list.end(); bus_itr++) {
			(*bus_itr)->run(&buses_at_busstop);
		}
	}
	
	// 全体の待ち時間を出力
	std::cout << "Total Waiting Time = " << total_waiting_time << std::endl;

	return app.exec();
}
