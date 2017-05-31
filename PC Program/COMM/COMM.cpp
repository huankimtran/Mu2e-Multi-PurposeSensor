#include "./COMM.h"
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
string COMM::getASCIIMess(){
	int messSize;
	unsigned char dta;
	string mess="";
	while(true){
		if(!RS232_PollComport(ARPORT,&dta,1))
			continue;
		switch(dta){
			case START_HEADING:
			continue;
			case END_TEXT:
			return mess;
			default:
			mess+=dta;				
		}
	}
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
	char* buff=(char*)calloc(MAX_BUFF_POS+1,sizeof(char));
	string remain;
	ofstream file;
	numbFile=0;
	if(sendCommand(ARDUINO_PULL)==COMM_ERROR)
		return COMM_ERROR;
	usleep(COMMAND_DELAY);
	numbFile=getAns();
	cout<<"Number of files:"<<numbFile<<endl;
	if(!numbFile)
		return numbFile;
	count=0;
	currSize=0;			
	for(int i=1;i<=numbFile;i++){
		file.open(path+to_string(i),ios_base::out|ios_base::binary);
		if(!currSize)		
			fileSize=stoi(getASCIIMess());
		else{
			//Remark: Once the program reaches here, the first element of the buff is HEADING_START
			//Check if the remain data in the buffer contents the file size information
			char * ETpos=(char*)memchr(buff,END_TEXT,currSize);
			while(ETpos==NULL){
				//Put the remain data to temporary memory
				char * tmp=(char*)calloc(currSize,sizeof(char));
				memcpy(tmp,buff,currSize);
				//Get new data to look for the END_TEXT, which is the place where file size data end
				int newDtaSize=RS232_PollComport(ARPORT,(unsigned char*)buff,MAX_BUFF_POS+1);
				if(newDtaSize+currSize>MAX_BUFF_POS+1)
					buff=(char*)realloc(buff,newDtaSize+currSize);
				//Shift the new data forward find place to squeze the old data into buff
				memmove(buff+currSize,buff,newDtaSize);
				//Squeze the old data to buff
				memcpy(buff,tmp,currSize);
				free(tmp);
				currSize+=newDtaSize;
				ETpos=(char*)memchr(buff,END_TEXT,currSize);
			}
			fileSize=stoi(string(buff+1,((ETpos-buff)-1)));	//buff+1 is to ignore the HEADING_START
			memmove(buff,ETpos+1,currSize-(ETpos-buff+1));	//ETpos+1 is to ignore the END_TEXT
			currSize-=(ETpos-buff+1);
		}
		count=0;
		while(count<fileSize){
			if(!currSize)
				currSize=RS232_PollComport(ARPORT,(unsigned char*)buff,MAX_BUFF_POS+1);				
			//If the message contain data of 2 files
			if(count+currSize>fileSize){
				//write the rest of the first file
				char *tmp= (char*)calloc(fileSize-count,sizeof(char));
				memcpy(tmp,buff,fileSize-count);
				file.write((const char*)tmp,fileSize-count);
				free(tmp);				
				//Delete the data of the first file in the buff
				memmove(buff,buff+(fileSize-count),currSize-(fileSize-count));
				currSize-=(fileSize-count);
				break;				
			}else{
				count+=currSize;
				file.write((const char*)buff,currSize);
				currSize=0;	
			}
		}
		file.close();		
	}
	free(buff);
	return numbFile;
}
int COMM::sendCommand(int com){
	if(ARPORT==COMM_ERROR)
		return COMM_ERROR;
	if(RS232_SendByte(ARPORT,com)){
		RS232_CloseComport(ARPORT);
		return COMM_ERROR;
	}	
	return 1;
}
int COMM::getAns(){
	unsigned char com;
	usleep(COMMAND_DELAY);
	return RS232_PollComport(ARPORT,&com,1)?com:COMM_ERROR;
}
