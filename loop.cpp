#include "loop.h"
#include "passenger.h"
#include "bus.h"
#include "field.h"
#include <QApplication>

MainLoop::MainLoop(const std::list<std::shared_ptr<passenger>>& passengers, const std::list<std::shared_ptr<bus>>& buses,
	const std::unordered_multimap<Location, const bus*, pair_hash>& bus_busstop_map, const std::unordered_map<unsigned int,Location>& busstop_location, unsigned int N)
	: passenger_list(passengers), bus_list(buses), buses_at_busstop(bus_busstop_map), field(std::make_shared<Field>(passenger_list)), scene(),
	elapsed_text(nullptr), waiting_text(nullptr), population_text(nullptr), total_waiting_time(0), elapsed_frame(0)
{
	// �V�[���̍쐬
	QRectF scene_rect(-300, -300, 600, 600);
	scene.setSceneRect(scene_rect);
	scene.setItemIndexMethod(QGraphicsScene::NoIndex);

	// �摜�̃��[�h&�ϊ�
	QSize image_size(scene_rect.width() / N, scene_rect.height() / N);
	QImage bus_image;
	QPixmap busstop_pixmap;
	{
		QImage bus_image_raw("../traffic_simulator/bus.png");
		QImage busstop_image("../traffic_simulator/busstop.png");
		bus_image = bus_image_raw.scaled(image_size, Qt::KeepAspectRatio);
		busstop_pixmap.convertFromImage(busstop_image.scaled(image_size, Qt::KeepAspectRatio));
	}

	// �eitem�̓o�^
	scene.addItem(new Field_qt(field, N, passenger_list.size(), scene_rect));
	for (auto itr : busstop_location)
	{
		Location loc = itr.second;
		QGraphicsPixmapItem* busstop_item = new QGraphicsPixmapItem(busstop_pixmap);
		busstop_item->setPos(scene_rect.x() + (loc.second - 1) * image_size.width(), scene_rect.y() + (loc.first - 1) * image_size.height());
		scene.addItem(busstop_item);
	}
	for (auto itr : bus_list){ scene.addItem(new bus_qt(itr, bus_image, N, scene_rect)); }
	elapsed_text = scene.addSimpleText(QString().setNum(elapsed_frame));
	QFont waiting_font;
	waiting_font.setBold(true);
	waiting_font.setPointSizeF(waiting_font.pointSizeF() + 8.f);
	waiting_text = scene.addSimpleText(QString().setNum(total_waiting_time), waiting_font);
	population_text = scene.addSimpleText(QString().setNum(0));

	elapsed_text->setPos(-elapsed_text->boundingRect().width() / 2, scene_rect.top() + elapsed_text->boundingRect().height());
	waiting_text->setPos(-waiting_text->boundingRect().width() / 2, -waiting_text->boundingRect().height() / 2);
	population_text->setPos(-population_text->boundingRect().width() / 2, -population_text->boundingRect().height() - waiting_text->boundingRect().height() / 2);

	// �V�[���̍X�V
	scene.advance();
}

QGraphicsScene* MainLoop::get_scene() { return &scene; }
unsigned int MainLoop::get_waiting_time() const { return total_waiting_time; }

void MainLoop::run()
{
	elapsed_frame++;

	// ��q��������菈��
	if (!passenger_list.empty())
	{
		// �t�B�[���h�����N���A
		field->clear();

		// �S��q�̍X�V����
		for (auto passenger_itr = passenger_list.begin(); passenger_itr != passenger_list.end(); )
		{
			auto passenger_ptr = (*passenger_itr)->update(buses_at_busstop);
			if (passenger_ptr){ field->register_passenger(passenger_ptr); }
			else
			{
				// �ړI�n�ɓ��������̂ŁA�S�̂̑҂����Ԃ����Z
				total_waiting_time += (*passenger_itr)->get_waiting_time();

				// �S�̂̋q���X�g���珜�O
				passenger_itr = passenger_list.erase(passenger_itr);
				continue;
			}
			passenger_itr++;
		}

		// �J�E���^�[������̍X�V
		elapsed_text->setText(QString().setNum(elapsed_frame));
		waiting_text->setText(QString().setNum(total_waiting_time));
		population_text->setText(QString().setNum(passenger_list.size()));

		elapsed_text->setPos(-elapsed_text->boundingRect().width() / 2, scene.sceneRect().top() + elapsed_text->boundingRect().height());
		waiting_text->setPos(-waiting_text->boundingRect().width() / 2, -waiting_text->boundingRect().height() / 2);
		population_text->setPos(-population_text->boundingRect().width() / 2, -population_text->boundingRect().height() - waiting_text->boundingRect().height() / 2);

		if (passenger_list.empty()) { waiting_text->setBrush(Qt::red); }

		// ��x�o�X���̃o�X�����N���A
		buses_at_busstop.clear();

		// �o�X��i�߂�
		for (auto bus_itr : bus_list) { bus_itr->run(&buses_at_busstop); }

		// �V�[���̍X�V
		scene.advance();
	}

	return;
}
