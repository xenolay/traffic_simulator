#include "header.h"
#include <list>
int main(){
	int i; //�l������
	int totalwaitingtime;
	std::list<passenger> L;
	std::vector<bus> V;
	for(i=0; i<passenger_num; i++){
		test.cpp;
		L.pushback();
	}
	for(i=0;i<buss_num;i++){
		buss.cpp;
	}
	while(!L.empty()){ // ��q������Ȃ��

		//�o�X��i�߂�
		for (i=0;i<size(V);i++){
			if(V(i).current_x < V(i).destination_x){
				V(i).current_x++;
			} else if (V(i).current_x > V(i).destination_x) {
				V(i).current_x--;
			} else if (V(i).current_y < V(i).destination_y){
				V(i).current_y++;
			} else if (V(i).current_y > V(i).destination_y){
				V(i).current_y--;
			}
		}
		//�l���~�낷
		// �֐�whichbus�͖���`�Ȃ̂ł��Ƃ�
	std::list<passenger>::iterator passenger_itr;
	std::vector<bus>::iterator bus_itr;
	for(passenger_itr = L.begin(); passenger_itr != L.end(); passenger_itr++){
		if(*passenger_itr.current == *passenger_itr.destination){
			totalwaitingtime += *passenger_itr.waitingtime;
			L.remove(*passenger_itr);
			whichbus(*passenger_itr).current--;
		}
	}
	//�l���悹��

	for(passenger_itr = L.begin(); passenger_itr != L.end(); passenger_itr++){
		for(bus_itr = V.begin(); bus_itr != V.end(); bus_itr++){
			if(*passenger_itr.current == *bus_itr.current && *bus_itr.is_going_to(*passenger_itr.destination)){
				*passenger_itr.boarding_bus = *bus_itr.ID;
				bus_itr.current++;
			}
		}
	}

	//���ĂȂ��l�̃J�E���^���グ��
	for(itr = L.begin(); itr != L.end(); itr++){
		if(*itr.boarding_bus == NULL){
			*itr.waitingtime++;
		}
	}
	std::cout << totalwaitingtime;
	return 0;
	}
