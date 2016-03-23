#include "loop.h"
#include "passenger.h"
#include "bus.h"
#include <QApplication>
#include "busstop.h"

MainLoop::MainLoop(const std::list<std::shared_ptr<passenger>>& passengers, const std::list<std::shared_ptr<bus>>& buses,
    const std::unordered_multimap<Location, const bus*, pair_hash>& bus_busstop_map,  const std::unordered_map<unsigned int,Location>& busstops,unsigned int gridN)
    : passenger_list(passengers), bus_list(buses), buses_at_busstop(bus_busstop_map), busstop_location(busstops),scene(), total_waiting_time(0)
{
	// バス画像のロード
    QImage bus_image("C:/Users/t2ladmin/Desktop/git4/bus.png");
    QImage busstop_image("C:/Users/t2ladmin/Desktop/git4/busstop.gif");
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
			if ((*passenger_itr)->update(buses_at_busstop))
			{
				// 目的地に到着したので、全体の待ち時間を加算
				total_waiting_time += (*passenger_itr)->get_waiting_time();

				// 全体の客リストから除外
				passenger_itr = passenger_list.erase(passenger_itr);
				continue;
			}
			passenger_itr++;
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
