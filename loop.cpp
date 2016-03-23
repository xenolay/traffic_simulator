#include "loop.h"
#include "passenger.h"
#include "bus.h"
#include "place.h"
#include <QApplication>
#include "busstop.h"

MainLoop::MainLoop(const std::list<std::shared_ptr<passenger>>& passengers, const std::list<std::shared_ptr<bus>>& buses,
    const std::unordered_multimap<Location, const bus*, pair_hash>& bus_busstop_map, const std::unordered_map<unsigned int,Location>& busstops, unsigned int gridN, Matrix &placelist)
    : passenger_list(passengers), bus_list(buses), buses_at_busstop(bus_busstop_map), busstop_location(busstops), scene(), total_waiting_time(0), gridN(gridN), place_list(placelist)
{
    // バス画像のロード
    QImage bus_image("/Users/xenolay/Dev/mayfes/traffic_simulator/bus.png");
    QImage busstop_image("/Users/xenolay/Dev/mayfes/traffic_simulator/busstop.gif");
    QImage place_green("/Users/xenolay/Dev/mayfes/traffic_simulator/green.png");
    QImage place_yellow("/Users/xenolay/Dev/mayfes/traffic_simulator/yellow.png");
    QImage place_red("/Users/xenolay/Dev/mayfes/traffic_simulator/red.png");

    // シーンの作成
    scene.setSceneRect(-300, -300, 600, 600);
    scene.setItemIndexMethod(QGraphicsScene::NoIndex);
    for (auto itr : bus_list){ scene.addItem(new bus_qt(itr, bus_image, gridN, scene.sceneRect())); }
    for(auto itr2:busstop_location){scene.addItem(new busstop_qt(itr2.second,busstop_image,gridN,scene.sceneRect()));}
    // シーンの更新
    scene.advance();
}

QGraphicsScene* MainLoop::get_scene() { return &scene; }
unsigned int MainLoop::get_waiting_time() const { return total_waiting_time; }

void MainLoop::run()
{
    // 乗客がいる限り処理
    if (!passenger_list.empty()) {
        // 全乗客の更新処理
        for (auto passenger_itr = passenger_list.begin(); passenger_itr != passenger_list.end(); ) {
            place_list[(*passenger_itr)->get_current_location().first - 1][(*passenger_itr)->get_current_location().second - 1]--;
            if ((*passenger_itr)->update(buses_at_busstop))
            {
                // 目的地に到着したので、全体の待ち時間を加算
                total_waiting_time += (*passenger_itr)->get_waiting_time();

                // 全体の客リストから除外
                passenger_itr = passenger_list.erase(passenger_itr);
                continue;
            }
            place_list[(*passenger_itr)->get_current_location().first - 1][(*passenger_itr)->get_current_location().second - 1]++;
            passenger_itr++;
        }

        // どこで何人待っているかのを表示
        for (unsigned int i = 0; i < gridN; i++)
        {
            for (unsigned int j = 0; j < gridN; j++){
                std::cout << i + 1 << "," << j + 1 << ": waiting(called in loop.cpp) " << place_list[i][j] << std::endl;
            }
        }

        // 一度バス停上のバス情報をクリア
        buses_at_busstop.clear();

        // バスを進める
        for (auto bus_itr = bus_list.begin(); bus_itr != bus_list.end(); bus_itr++) {
            (*bus_itr)->run(&buses_at_busstop);
        }

        // シーンの更新
        scene.advance();
    }

    return;
}
