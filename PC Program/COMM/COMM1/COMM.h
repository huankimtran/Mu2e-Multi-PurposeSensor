#ifndef COMM_H
#define COMM_H
#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
//---------COMM-CONNECTION------------------
#define MAX_PORT 25
#define BAUD_RATE 9600
#define COMM_ERROR -1
#define COMMAND_DELAY 10000
#define MAX_BUFF_POS 4095
//---------COMMANDS---------------------
#define ARDUINO_SALUTE 0
#define ARDUINO_PULL 1
#define ARDUINO_ANS 254
//---------PROTOCOL------------------
#define START_HEADING 1
#define START_TEXT  2 
#define END_TEXT 3
#define END_TRANS 4
//-----------------------------------
#include "rs232.h"
using namespace std;
class COMM{
	public:
	int ARPORT;
	string path;
	int getArduinoPort();
	int pullData();
	private:
	int sendCommand(int com);
	int getAns();
	string getASCIIMess();
}
#endif
