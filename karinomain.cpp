#include "header.h"
#include<list>
int main(){
	int i; //�l������
	list<�l�̃N���X> L;
	vector<�o�X�̃N���X> V;
	for(i=0;i<passenger_num;i++){
		test.cpp;
		L.pushback;
	}
	for(i=0;i<buss_num;i++){
		buss.cpp;
	}
	while(��q������){
		//�o�X��i�߂�
		for (i=0;i<size(V);i++){
			if(V(i).current_x<V(i).destination_x){
				V(i).current_x++;
			}
			else{
				if (V(i).current_y<V(i).destination_y){
					V(i).current_y++;
				}
			}
		}
		//�l���~�낷
	list<�l�̃N���X>::iterator itr;
	for(itr=L.begin();itr!=L.end();itr++){
		if(*itr.current==*itr.destination){
			totalwaitingtime+=*itr.waitingtime;
			L.remove(*itr);
			�o�X.current--;
		}
	}
	//�l���悹��
	for(itr=L.begin();itr!=L.end();itr++){
		if(*itr.current==�o�X��current){
			*itr.notteru=true;
			�o�X.current++;
		}
	}
	//���ĂȂ��l�̃J�E���^���グ��
	for(itr=L.begin();itr!=L.end();itr++){
		if(*itr.notteru==false){
			*itr.waitingtime++;
		}
	}
	cout<<totalwaitingtime;
	return 0;
	}

