#include "loop.h"
#include "passenger.h"
#include "bus.h"
#include "field.h"
#include <QApplication>

MainLoop::MainLoop(const std::list<std::shared_ptr<passenger>>& passengers, const std::list<std::shared_ptr<bus>>& buses,
	const std::unordered_multimap<Location, const bus*, pair_hash>& bus_busstop_map, const std::unordered_map<unsigned int,Location>& busstop_location, unsigned int gridN)
	: passenger_list(passengers), bus_list(buses), buses_at_busstop(bus_busstop_map), field(std::make_shared<Field>(passenger_list)), scene(), total_waiting_time(0), N(gridN)
{
	// �V�[���̍쐬
	QRectF scene_rect(-300, -300, 600, 600);
	scene.setSceneRect(scene_rect);
	scene.setItemIndexMethod(QGraphicsScene::NoIndex);

	// �摜�̃��[�h&�ϊ�
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
	
	// �eitem�̓o�^
	scene.addItem(new Field_qt(field, N, passenger_list.size(), scene_rect));
	for (auto itr : busstop_location)
	{
		Location loc = itr.second;
		QGraphicsPixmapItem* busstop_item = new QGraphicsPixmapItem(busstop_pixmap);
		busstop_item->setPos(scene_rect.x() + (loc.second - 1) * image_width, scene_rect.y() + (loc.first - 1) * image_height);
		scene.addItem(busstop_item);
	}
	for (auto itr : bus_list){ scene.addItem(new bus_qt(itr, bus_image, N, scene_rect)); }
    
	// �V�[���̍X�V
	scene.advance();
}

QGraphicsScene* MainLoop::get_scene() { return &scene; }
unsigned int MainLoop::get_waiting_time() const { return total_waiting_time; }

void MainLoop::run()
{
	// ��q��������菈��
	if (!passenger_list.empty())
	{
		// �t�B�[���h�����N���A
		field->clear();

		// �S��q�̍X�V����
		for (auto passenger_itr = passenger_list.begin(); passenger_itr != passenger_list.end(); )
		{
			auto passenger_ptr = (*passenger_itr)->update(buses_at_busstop);
			if (!passenger_ptr)
			{
				// �ړI�n�ɓ��������̂ŁA�S�̂̑҂����Ԃ����Z
				total_waiting_time += (*passenger_itr)->get_waiting_time();

				// �S�̂̋q���X�g���珜�O
				passenger_itr = passenger_list.erase(passenger_itr);
				continue;
			}
			else
			{
				field->register_passenger(passenger_ptr);
			}
			passenger_itr++;
		}

		// ��x�o�X���̃o�X�����N���A
		buses_at_busstop.clear();

		// �o�X��i�߂�
		for (auto bus_itr : bus_list) { bus_itr->run(&buses_at_busstop); }

		// �V�[���̍X�V
		scene.advance();
	}

	return;
}
