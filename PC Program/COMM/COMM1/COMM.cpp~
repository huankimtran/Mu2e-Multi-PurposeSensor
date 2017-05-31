#include <COMM.h>
COMM::COMM(){
	path="";
	ARPORT=getArduinoPort();
}
int COMM::getArduinoPort(){
	unsigned char back=100;
	for(int i=0;i<=MAX_PORT;i++){
		cout<<"Checking port: "<<i<<endl;
		if(!RS232_OpenComport(i,BAUD_RATE,"8N1")){
//------------------------NOTICE-------------------------------------
//--------------ONCE-PORT-HAS-BEEN-OPENNED---------------------------
//--------------IMMIDIATELY-DELAY-THE-PROGRAM-FOR-AT-LEAST-200ms-----
			usleep(2000000);
//--------------IF-NOT-THE-RESPONSE-FROM-ARDUINO-WILL-BE-LAGGED------
			if(RS232_SendByte(i,ARDUINO_SALUTE)){
				RS232_CloseComport(i);
				continue;
			}
			usleep(1000000);
			if(!RS232_PollComport(i,&back,1)){
				RS232_CloseComport(i);
				continue;
			}
			if(back==ARDUINO_ANS)
				return i;
			else
				RS232_CloseComport(i);
		}
	}
	return COMM_ERROR;
}
/*
 *This function request the arduino to send all files on the SDCard to pc
 *Arduino can sends two type of message raw data or ASCII data
 *an ASCII data message consists of START_HEADING + ASCII data + END_TEXT. This will be denoted as ASCII(data)
 *Ex: ASCII(12) = START_HEADING 49 50 END_TEXT
 *raw data message consists of ASCII(size of raw data in byte) + data in byte . This will be denoted as RAW(size)
 *Ex: RAW(12) = START_HEADING 49 50 END_TEXT + 12 bytes
 *Arduino sends all data file consecutively. Files of which names are smaller (when convert to int)  will be sent first
 *Structure of conversation:
 *-PC:ARDUINO_PULL
 *-AR: 1 byte represent the number of file (file on the SDCard is named according to the order of creation)
 *-AR: RAW(sizeoffile(1)) + RAW(sizeoffile(2))+ ...
 */
int COMM::pullData(){
	int numbFile, fileSize,count,currSize;
	unsigned char com,buff[MAX_BUFF_POS+1];
	ofstream file;
	numbFile=0;
	if(sendCommand(ARDUINO_PULL)==COMM_ERROR)
		return COMM_ERROR;
	while((numbFile=getAns())!=COMM_ERROR);
	if(!numbFile)
		return numbFile;
	for(int i=0;i<numbFile;i++){
		file.open(path+to_string(i),std::out|std:binary);
		fileSize=stoi(GetASCIIMess());
		count=0;
		while(count<fileSize){
			currSize=RS232_PollComport(ARPORT,buff,MAX_BUFF_POS+1);
			count+=currSize;
			file.write(buff,currSize);		
		}
		file.close();		
	}
	retrun numbFile;
}
string COMM::getASCIIMess(){
	unsigned char dta;
	string mess="";
	while(true){
		RS232_PollComport(ARPORT,&dta,1);
		switch(dta){
			case 0:
			case START_HEADING:
			continue;
			case END_TEXT:
			return mess;
			default:
			mess+=dta;				
		}
	}
}
int COMM::sendCommand(int com){
	if(ARPORT==COMM_ERROR)
		return COMM_ERROR
	if(RS232_SendByte(ARPORT,com)){
		RS232_CloseComport(i);
		return COMM_ERROR
	}	
	return 1;
}
int COMM::getAns(){
	unsigned char com;
	usleep(COMMAND_DELAY);
	return RS232_PollComport(ARPORT,&com,1)?com:COMM_ERROR;
}
