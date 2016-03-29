#include "loop.h"
#include "passenger.h"
#include "bus.h"
#include "field.h"
#include <QApplication>

MainLoop::MainLoop(const std::list<std::shared_ptr<passenger>>& passengers, const std::list<std::shared_ptr<bus>>& buses,
	const std::unordered_multimap<Location, const bus*, pair_hash>& bus_busstop_map, const std::unordered_map<unsigned int,Location>& busstop_location, unsigned int gridN)
	: passenger_list(passengers), bus_list(buses), buses_at_busstop(bus_busstop_map), field(std::make_shared<Field>(passenger_list)), scene(), total_waiting_time(0), N(gridN)
{
	// シーンの作成
	QRectF scene_rect(-300, -300, 600, 600);
	scene.setSceneRect(scene_rect);
	scene.setItemIndexMethod(QGraphicsScene::NoIndex);

	// 画像のロード&変換
	unsigned int image_width = scene_rect.width() / N;
	unsigned int image_height = scene_rect.height() / N;
	QImage bus_image;
	QPixmap busstop_pixmap;
	{
		QImage bus_image_raw("../traffic_simulator/bus.png");
		QImage busstop_image("../traffic_simulator/busstop.png");
		bus_image = bus_image_raw.scaled(image_width, image_height, Qt::KeepAspectRatio);
		busstop_pixmap.convertFromImage(busstop_image.scaled(image_width, image_height, Qt::KeepAspectRatio));
	}	
	
	// 各itemの登録
	scene.addItem(new Field_qt(field, N, passenger_list.size(), scene_rect));
	for (auto itr : busstop_location)
	{
		Location loc = itr.second;
		QGraphicsPixmapItem* busstop_item = new QGraphicsPixmapItem(busstop_pixmap);
		busstop_item->setPos(scene_rect.x() + (loc.second - 1) * image_width, scene_rect.y() + (loc.first - 1) * image_height);
		scene.addItem(busstop_item);
	}
	for (auto itr : bus_list){ scene.addItem(new bus_qt(itr, bus_image, N, scene_rect)); }
    
	// シーンの更新
	scene.advance();
}

QGraphicsScene* MainLoop::get_scene() { return &scene; }
unsigned int MainLoop::get_waiting_time() const { return total_waiting_time; }

void MainLoop::run()
{
	// 乗客がいる限り処理
	if (!passenger_list.empty())
	{
		// フィールド情報をクリア
		field->clear();

		// 全乗客の更新処理
		for (auto passenger_itr = passenger_list.begin(); passenger_itr != passenger_list.end(); )
		{
			auto passenger_ptr = (*passenger_itr)->update(buses_at_busstop);
			if (!passenger_ptr)
			{
				// 目的地に到着したので、全体の待ち時間を加算
				total_waiting_time += (*passenger_itr)->get_waiting_time();

				// 全体の客リストから除外
				passenger_itr = passenger_list.erase(passenger_itr);
				continue;
			}
			else
			{
				field->register_passenger(passenger_ptr);
			}
			passenger_itr++;
		}

		// 一度バス停上のバス情報をクリア
		buses_at_busstop.clear();

		// バスを進める
		for (auto bus_itr : bus_list) { bus_itr->run(&buses_at_busstop); }

		// シーンの更新
		scene.advance();
	}

	return;
}
