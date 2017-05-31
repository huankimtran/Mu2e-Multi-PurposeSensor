/*
 *This is the old version of the decoding program
 *Arduino is installed with source code : Maincode3
 *Arduino open SDCard, check for the file name META
 *If there is no file named META, create and write 1 into the file
 *From now on this file contain the number of data file existing in the SDCard
 *The data file naming convention is to use the number of data file to name it
 *For example if META contain number 5
 *The arduino will create the file name 6 and write data inside
 *If there is file name META, arduino reads the number writen in META file, increase that number by 1
 *And create the file with the name of the new number, then it logs data to that file
 */
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
#define LOW_BYTE 1
#define HIGH_BYTE 0
#define NUMBER_OF_DATA_FIELD 4
#define SENSIVITY 4096
const bool DISPLAY_UNIT=true;
const char* ACC_DATA_UNIT=" g";
const char* TEMP_DATA_UNIT=" C";
const string EXTENSION=".txt";
union Word{
	unsigned char byte[2];
	short word;
};
union DoubleWord{
	unsigned char byte[4];
	unsigned int dblWord;
};
float decodeTemp(short rawDta){
	return (float(rawDta)/340 + 36.53);
}
float decodeAcc(short rawDta,short sensivity){
	return (float(rawDta)/sensivity);
}
unsigned int combineDoubleWord(char *data){
/*	unsigned int fram=0;
	fram=(*data);
	fram=fram<<8;
	fram+=(*(data+1));
	fram=fram<<8;
	fram+=(*(data+2));
	fram=fram<<8;
	fram+=(*(data+3));
*/
	DoubleWord frame;
	for(int i=0;i<4;i++)
		frame.byte[i]=*(data+3-i);
	return frame.dblWord;
}
short combineShort(char *data){
/*	unsigned short fram=0;
	fram=(*data);
	fram=fram<<8;
	fram+=(*(data+1));
*/
	Word frame;
	frame.byte[1]=*data;
	frame.byte[0]=*(data+1);
	return frame.word;
}
int main(int argc,char *argv[]){
	ifstream inpf;
	ofstream outf;
	char raw,rawDta[4];
	short readingData,index,dtaSetSize;
	unsigned int timeStamp;
	Word a;
	if(argc<2)
		cout<<"No path found"<<endl;
	else{		
		inpf.open(argv[1],ifstream::binary);
		string outFileName(argv[1]);
		if(inpf.is_open()){
			outFileName+=EXTENSION;
			outf.open(outFileName.c_str(),ofstream::out);
			inpf.read(&raw,1);
			dtaSetSize=short(raw);
			index=0;
			while(!inpf.eof()){
				switch(index%NUMBER_OF_DATA_FIELD){
					case 0:
					inpf.read(rawDta,4);
					timeStamp=combineDoubleWord(rawDta);
					outf<<timeStamp<<endl;
					break;
					case 1:
					case 2:
					case 3:
					inpf.read(rawDta,2);
					readingData=combineShort(rawDta);
					outf<<decodeAcc(readingData,SENSIVITY)<<(DISPLAY_UNIT?ACC_DATA_UNIT:" ")<<endl;
					break;
					default:
					break;
				}
				if((++index)==dtaSetSize*NUMBER_OF_DATA_FIELD){
					index=0;
					if(!inpf.eof()){
						inpf.read(rawDta,2);
						readingData=combineShort(rawDta);
						outf<<decodeTemp(readingData)<<(DISPLAY_UNIT?TEMP_DATA_UNIT:" ")<<endl;
					}
				}
			}
			inpf.close();
			outf.close();
		}else
			cout<<"There is no file according to the given path"<<endl;
	}
	return 0;
}
