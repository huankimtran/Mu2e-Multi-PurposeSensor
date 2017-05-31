#include <iostream>
#include <sys/time.h>
#include "../Library/RS-232/rs232.h"
using namespace std;
#define BAUD_RATE 921600
#define KEY 100
#define HELLO 101
#define MAX_PORT 25
#define COMM_ERROR -1
#define MESSAGE_SIZE 1024000
#define USING_BUFF 10
#define MAX_BUFF 500
long int gettimestamp()  
{
	timeval tv;
	gettimeofday (&tv, NULL);
	return double (tv.tv_sec) + 0.000001 * tv.tv_usec;
}
int getArduinoPort(){
	unsigned char reply=HELLO;
	for(int i=0;i<=MAX_PORT;i++){
		cout<<"Checking port: "<<i<<endl;
		if(!RS232_OpenComport(i,BAUD_RATE,"8N1")){
//------------------------NOTICE-------------------------------------
//--------------ONCE-PORT-HAS-BEEN-OPENNED---------------------------
//--------------IMMIDIATELY-DELAY-THE-PROGRAM-FOR-AT-LEAST-200ms-----
			usleep(2000000);
//--------------IF-NOT-THE-RESPONSE-FROM-ARDUINO-WILL-BE-LAGGED------
			if(RS232_SendByte(i,HELLO)){
				RS232_CloseComport(i);
				continue;
			}
			usleep(1000000);
			if(!RS232_PollComport(i,&reply,1)){
				RS232_CloseComport(i);
				continue;
			}
			if(reply==HELLO)
				return i;
			else
				RS232_CloseComport(i);
		}
	}
	return COMM_ERROR;
}

int main(){
	int port=getArduinoPort();
	unsigned char dta[MESSAGE_SIZE];	
	long int beg,en,count,add;
	count=0;
	beg=gettimestamp();
	RS232_SendByte(port,KEY);
	do{
		add=RS232_PollComport(port,dta,MESSAGE_SIZE);
		count+=add;
		for(int i=0;i<add;i++)
			if((dta[i]!=KEY)&&(count!=1+MESSAGE_SIZE)){
				cout<<"ERROR"<<endl;
				break;
			}
	}while(count<=MESSAGE_SIZE);
	cout<<"Speed: "<<MESSAGE_SIZE/float(gettimestamp()-beg)<<" bps"<<endl;
	return 0;
}
