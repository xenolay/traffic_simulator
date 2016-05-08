#include "loop.h"
#include "passenger.h"
#include "bus.h"
#include "field.h"
#include "graph.h"
#include <fstream>
#include <sstream>
#include <random>
#include <QApplication>

CalcLoop::CalcLoop(const std::string& title, const std::list<std::shared_ptr<passenger>>& passengers, const std::list<std::shared_ptr<bus>>& buses, std::unordered_map<unsigned int, std::vector<Location>> route,
	const std::unordered_multimap<Location, const bus*, pair_hash>& bus_busstop_map, const std::unordered_map<unsigned int, Location>& busstop_location, unsigned int N)
	: passenger_list(passengers), bus_list(buses), buses_at_busstop(bus_busstop_map), field(std::make_shared<Field>(passenger_list)), scene(),
	elapsed_text(nullptr), waiting_text(nullptr), population_text(nullptr), total_waiting_time(0), elapsed_frame(0)
{
	// シーンの作成
	QRectF scene_rect(-300, -300, 600, 600);
	scene.setSceneRect(scene_rect);
	scene.setItemIndexMethod(QGraphicsScene::NoIndex);

	// 画像のロード&変換
	QSize image_size(scene_rect.width() / N, scene_rect.height() / N);
	QImage bus_image;
	QPixmap busstop_pixmap;
	{
		QImage bus_image_raw("../traffic_simulator/bus.png");
		QImage busstop_image("../traffic_simulator/busstop.png");
		bus_image = bus_image_raw.scaled(image_size, Qt::KeepAspectRatio);
		busstop_pixmap.convertFromImage(busstop_image.scaled(image_size, Qt::KeepAspectRatio));
	}

	// 経路図作成
	QPixmap busroute_pixmap(scene_rect.toRect().size());
	busroute_pixmap.fill();
	QPainter busroute_painter(&busroute_pixmap);
	for (unsigned int i = 0; i < route.size(); i++)
	{
		busroute_painter.setPen(QColor::fromHsvF(static_cast<double>(i) / route.size(), 1.0, 1.0));
		QVector<QLineF> lines;
		for (unsigned int j = 0; j < route[i].size() - 1; j++)
		{
			lines << QLineF((route[i].at(j).second - 0.5) * image_size.width()+ (i / 2 + i % 2)*pow(-1, i % 2), (route[i].at(j).first - 0.5) * image_size.height()+ (i / 2 + i % 2)*pow(-1, i % 2), (route[i].at(j).second - 0.5) * image_size.width()+ (i / 2 + i % 2)*pow(-1, i % 2), (route[i].at(j + 1).first - 0.5) * image_size.height()+ (i / 2 + i % 2)*pow(-1, i % 2));
			lines << QLineF((route[i].at(j).second - 0.5) * image_size.width()+ (i / 2 + i % 2)*pow(-1, i % 2), (route[i].at(j + 1).first - 0.5) * image_size.height()+ (i / 2 + i % 2)*pow(-1, i % 2), (route[i].at(j + 1).second - 0.5) * image_size.width()+ (i / 2 + i % 2)*pow(-1, i % 2), (route[i].at(j + 1).first - 0.5) * image_size.height()+ (i / 2 + i % 2)*pow(-1, i % 2));
		}
		busroute_painter.drawLines(lines);
	}
	QGraphicsPixmapItem* busroute_item = new QGraphicsPixmapItem(busroute_pixmap);
	busroute_item->setPos(scene_rect.x(), scene_rect.y());
	scene.addItem(busroute_item);

	// 各itemの登録
	scene.addItem(new Field_qt(field, N, passenger_list.size(), scene_rect));
	for (auto itr : busstop_location)
	{
		Location loc = itr.second;
		QGraphicsPixmapItem* busstop_item = new QGraphicsPixmapItem(busstop_pixmap);
		busstop_item->setPos(scene_rect.x() + (loc.second - 1) * image_size.width(), scene_rect.y() + (loc.first - 1) * image_size.height());
		scene.addItem(busstop_item);
	}
	for (auto itr : bus_list) { scene.addItem(new bus_qt(itr, bus_image, N, scene_rect)); }
	elapsed_text = scene.addSimpleText(QString().setNum(elapsed_frame));
	QFont waiting_font;
	waiting_font.setBold(true);
	waiting_font.setPointSizeF(waiting_font.pointSizeF() + 8.f);
	waiting_text = scene.addSimpleText(QString().setNum(total_waiting_time), waiting_font);
	population_text = scene.addSimpleText(QString().setNum(0));

	elapsed_text->setPos(-elapsed_text->boundingRect().width() / 2, scene_rect.top() + elapsed_text->boundingRect().height());
	waiting_text->setPos(-waiting_text->boundingRect().width() / 2, -waiting_text->boundingRect().height() / 2);
	population_text->setPos(-population_text->boundingRect().width() / 2, -population_text->boundingRect().height() - waiting_text->boundingRect().height() / 2);

	// シーンの更新
	scene.advance();

	// ビューの作成
	view.setScene(&scene);
	view.setRenderHint(QPainter::Antialiasing);
	view.setBackgroundBrush(Qt::white);
	view.setCacheMode(QGraphicsView::CacheBackground);
	view.setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::BoundingRectViewportUpdate);
	//view.setDragMode(QGraphicsView::ScrollHandDrag);
	view.setWindowTitle(QT_TRANSLATE_NOOP(QGraphicsView, title.c_str()));
	view.resize(scene_rect.adjusted(-5, -5, 5, 5).size().toSize());
	view.setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	view.setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	view.show();
}

unsigned int CalcLoop::get_waiting_time() const { return total_waiting_time; }

bool CalcLoop::run()
{
	elapsed_frame++;

	// 乗客がいる限り処理
	if (!passenger_list.empty())
	{
		// フィールド情報をクリア
		field->clear();

		// 全乗客の更新処理
		for (auto passenger_itr = passenger_list.begin(); passenger_itr != passenger_list.end(); )
		{
			auto passenger_ptr = (*passenger_itr)->update(buses_at_busstop);
			if (passenger_ptr) { field->register_passenger(passenger_ptr); }
			else
			{
				// 目的地に到着したので、全体の待ち時間を加算
				total_waiting_time += (*passenger_itr)->get_waiting_time();

				// 全体の客リストから除外
				passenger_itr = passenger_list.erase(passenger_itr);
				continue;
			}
			passenger_itr++;
		}

		// カウンター文字列の更新
		elapsed_text->setText(QString().setNum(elapsed_frame));
		waiting_text->setText(QString().setNum(total_waiting_time));
		population_text->setText(QString().setNum(passenger_list.size()));

		elapsed_text->setPos(-elapsed_text->boundingRect().width() / 2, scene.sceneRect().top() + elapsed_text->boundingRect().height());
		waiting_text->setPos(-waiting_text->boundingRect().width() / 2, -waiting_text->boundingRect().height() / 2);
		population_text->setPos(-population_text->boundingRect().width() / 2, -population_text->boundingRect().height() - waiting_text->boundingRect().height() / 2);

		if (passenger_list.empty()) { waiting_text->setBrush(Qt::red); }

		// 一度バス停上のバス情報をクリア
		buses_at_busstop.clear();

		// バスを進める
		for (auto bus_itr : bus_list) { bus_itr->run(&buses_at_busstop); }

		// シーンの更新
		scene.advance();

		return true;
	}

	return false;
}

void CalcLoop::show() { return view.show(); }
void CalcLoop::hide() { return view.hide(); }

InitLoop::InitLoop(const std::string& title, const std::string& filename, unsigned int passenger_num, unsigned int bus, unsigned int capacity)
	:gridN(0), busstop_location(), bus_route(), passenger_list(), bus_list(), buses_at_busstop(), window(), label(), spinboxes(nullptr), button(), bus_num(bus), bus_capacity(capacity), input_end(false), init_end(false)
{
	// setting fileの読み込み
	std::ifstream setting_file(filename);
	if (setting_file.fail())
	{
		std::cerr << "The setting file wasn't found." << std::endl;
		throw std::runtime_error("file not found.");
	}

	// 全体のサイズ(1辺)
	setting_file >> gridN;

	// バス停情報の読み込み(busstop_location)
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

			if (x == 0 || y == 0 || x > gridN || y > gridN) {
				std::cerr << "The " << index << "-busstop has invalid location, so the busstop wasn't registered." << std::endl;
				continue;
			}
			// 読み込んだ情報の登録
			busstop_location.insert(std::pair<unsigned int, Location>(index, Location(x, y)));
			busstop_prob.at(i) = prob;
		}

		for (unsigned int i = 1; i < busstop_num; i++) { busstop_prob[i] += busstop_prob[i - 1]; }	// 累積確率分布もどきに変換
	}

	// バス路線図の読み込み(bus_route)
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

	// 乗客リストの作成(passenger_list)
	{
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

	// バス配分入力用のwindow作成
	QVBoxLayout vlayout;
	label.setText((std::string(u8"バス残り") + std::to_string(bus_num) + u8"台").c_str());
	vlayout.addWidget(&label);

	spinboxes = new QSpinBox [bus_route.size()];
	std::string spinbox_prefix(u8"ルート");
	for (unsigned int i = 0; i < bus_route.size(); i++)
	{
		spinboxes[i].setPrefix((spinbox_prefix + std::to_string(i) + ":").c_str());
		spinboxes[i].setSuffix(u8"台");
		spinboxes[i].setRange(0, bus_num);
		vlayout.addWidget(&spinboxes[i]);
		QObject::connect(&spinboxes[i], SIGNAL(valueChanged(int)), this, SLOT(check(int)));
	}

	button.setText("START");
	button.setDisabled(true);
	vlayout.addWidget(&button);
	QObject::connect(&button, SIGNAL(clicked()), this, SLOT(confirmed()));

	window.setLayout(&vlayout);
	window.setWindowTitle(title.c_str());
	window.setFixedSize(260,200);
	window.show();

	return;
}

InitLoop::~InitLoop(){ delete [] spinboxes; }

void InitLoop::confirmed()
{
	unsigned int sum = 0;
	for (unsigned int i = 0; i < bus_route.size(); i++) { sum += spinboxes[i].value(); }
	if (sum == bus_num)
	{
		input_end = true;
		window.hide();
	}
	return;
}

void InitLoop::check(int)
{
	unsigned int sum = 0;
	for (unsigned int i = 0; i < bus_route.size(); i++) { sum += spinboxes[i].value(); }
	for (unsigned int i = 0; i < bus_route.size(); i++) { spinboxes[i].setMaximum((bus_num - sum) + spinboxes[i].value()); }
	label.setText((std::string(u8"バス残り") + std::to_string(bus_num - sum) + u8"台").c_str());
	if (sum == bus_num) { button.setEnabled(true); }
	else { button.setDisabled(true); }
	return;
}

bool InitLoop::run()
{
	if (input_end && !init_end)
	{
		// バス配置(bus_list, buses_at_busstop_list)
		std::random_device rnd;
		std::cout << "Bus Distribution:" << std::endl;
		for (unsigned int i = 0; i < bus_route.size(); i++)
		{
			std::cout << "<Route" << i << ">" << std::endl;
			std::uniform_int_distribution<unsigned int> rand(0, static_cast<unsigned int>(bus_route[i].size() - 1));
			for (unsigned int j = 0; j < spinboxes[i].value(); j++)
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
		init_end = true;
		return false;
	}

	return true;
}

void InitLoop::show() { return window.show(); }
void InitLoop::hide() { return window.hide(); }

std::shared_ptr<CalcLoop> InitLoop::genCalcLoop() const
{
	if(init_end){ return std::make_shared<CalcLoop>(window.windowTitle().toStdString(), passenger_list, bus_list, bus_route, buses_at_busstop, busstop_location, gridN); }
	return nullptr;
}


MainLoop::MainLoop(const std::string& title, const std::string& filename, unsigned int passenger_num, unsigned int bus_num, unsigned int bus_capacity)
	:init_loop(std::make_shared<InitLoop>(title, filename, passenger_num, bus_num, bus_capacity)), calc_loop(nullptr), state(LoopState::INIT), result(0)
{
	init_loop->show();
}

void MainLoop::run()
{
	switch (state)
	{
	case MainLoop::LoopState::INIT:
		if (!init_loop) { throw std::runtime_error(""); }
		if (!init_loop->run())
		{
			init_loop->hide();
			calc_loop = init_loop->genCalcLoop();
			calc_loop->show();
			state = LoopState::CALC;
		}
		break;

	case MainLoop::LoopState::CALC:
		if (!calc_loop) { throw std::runtime_error(""); }
		if (!calc_loop->run())
		{
			result = calc_loop->get_waiting_time();
			state = LoopState::END;
		}
		break;

	case MainLoop::LoopState::END:
		break;

	default:
		throw std::runtime_error("");
		break;
	}

	return;
}

unsigned int MainLoop::get_result() const { return result; }
