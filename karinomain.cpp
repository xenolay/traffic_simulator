#include "header.h"
#include<list>
int main(){
	int i; //人をつくる
	list<人のクラス> L;
	vector<バスのクラス> V;
	for(i=0;i<passenger_num;i++){
		test.cpp;
		L.pushback;
	}
	for(i=0;i<buss_num;i++){
		buss.cpp;
	}
	while(乗客がいる){
		//バスを進める
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
		//人を降ろす
	list<人のクラス>::iterator itr;
	for(itr=L.begin();itr!=L.end();itr++){
		if(*itr.current==*itr.destination){
			totalwaitingtime+=*itr.waitingtime;
			L.remove(*itr);
			バス.current--;
		}
	}
	//人を乗せる
	for(itr=L.begin();itr!=L.end();itr++){
		if(*itr.current==バスのcurrent){
			*itr.notteru=true;
			バス.current++;
		}
	}
	//乗れてない人のカウンタを上げる
	for(itr=L.begin();itr!=L.end();itr++){
		if(*itr.notteru==false){
			*itr.waitingtime++;
		}
	}
	cout<<totalwaitingtime;
	return 0;
	}

