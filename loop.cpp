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
    // �o�X�摜�̃��[�h
    QImage bus_image("/Users/xenolay/Dev/mayfes/traffic_simulator/bus.png");
    QImage busstop_image("/Users/xenolay/Dev/mayfes/traffic_simulator/busstop.gif");
    QImage place_green("/Users/xenolay/Dev/mayfes/traffic_simulator/green.png");
    QImage place_yellow("/Users/xenolay/Dev/mayfes/traffic_simulator/yellow.png");
    QImage place_red("/Users/xenolay/Dev/mayfes/traffic_simulator/red.png");

    // �V�[���̍쐬
    scene.setSceneRect(-300, -300, 600, 600);
    scene.setItemIndexMethod(QGraphicsScene::NoIndex);
    for (auto itr : bus_list){ scene.addItem(new bus_qt(itr, bus_image, gridN, scene.sceneRect())); }
    for(auto itr2:busstop_location){scene.addItem(new busstop_qt(itr2.second,busstop_image,gridN,scene.sceneRect()));}
    // �V�[���̍X�V
    scene.advance();
}

QGraphicsScene* MainLoop::get_scene() { return &scene; }
unsigned int MainLoop::get_waiting_time() const { return total_waiting_time; }

void MainLoop::run()
{
    // ��q��������菈��
    if (!passenger_list.empty()) {
        // �S��q�̍X�V����
        for (auto passenger_itr = passenger_list.begin(); passenger_itr != passenger_list.end(); ) {
            place_list[(*passenger_itr)->get_current_location().first - 1][(*passenger_itr)->get_current_location().second - 1]--;
            if ((*passenger_itr)->update(buses_at_busstop))
            {
                // �ړI�n�ɓ��������̂ŁA�S�̂̑҂����Ԃ����Z
                total_waiting_time += (*passenger_itr)->get_waiting_time();

                // �S�̂̋q���X�g���珜�O
                passenger_itr = passenger_list.erase(passenger_itr);
                continue;
            }
            place_list[(*passenger_itr)->get_current_location().first - 1][(*passenger_itr)->get_current_location().second - 1]++;
            passenger_itr++;
        }

        // �ǂ��ŉ��l�҂��Ă��邩�̂�\��
        for (unsigned int i = 0; i < gridN; i++)
        {
            for (unsigned int j = 0; j < gridN; j++){
                std::cout << i + 1 << "," << j + 1 << ": waiting(called in loop.cpp) " << place_list[i][j] << std::endl;
            }
        }

        // ��x�o�X���̃o�X�����N���A
        buses_at_busstop.clear();

        // �o�X��i�߂�
        for (auto bus_itr = bus_list.begin(); bus_itr != bus_list.end(); bus_itr++) {
            (*bus_itr)->run(&buses_at_busstop);
        }

        // �V�[���̍X�V
        scene.advance();
    }

    return;
}
