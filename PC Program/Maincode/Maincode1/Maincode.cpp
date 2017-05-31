#include <iostream>
#include <string>
#include "../COMM/COMM.h"
#define SAY_HELLO 4
#define SET_WORKING_DIR 3
#define PULL_DATA 2
#define DECODE 1
#define QUIT 0
const char* MENU="4 - Say hello to Arduino\n3 - Set working directory\n2 - Pull out data to working directory\n1 - Decode the data\n0 - Quit";
COMM comm;
int main(int argn,char* arg[]){
	int choice;
	do{
		cout<<"Welcome to Mu2e-TempHud program:"<<endl;
		cout<<MENU<<endl;
		cout<<"What do you want to do: ";cin>>choice;
		switch(choice){
			case SAY_HELLO:
			cout<<"Arduino is at port:"<<comm.ARPORT<<endl;	
			break;
			case SET_WORKING_DIR:
			break;
			case PULL_DATA:
			comm.pullData();
			break;
			case DECODE:
			break;
			default:
			cout<<"Your choice does not match any commands, try again"<<endl;			
			case QUIT:
			continue;
		}
	}while(choice);
}
