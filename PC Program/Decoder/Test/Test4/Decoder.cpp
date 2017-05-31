/*
 *This is the old version of the decoding program
 *Arduino is installed with source code : Arduino Program/MainCode/MainCode3/Maincode3.ino
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
const char* TEMP_DATA_UNIT=" C";
const string TEMP_EXTENSION=".tmp";
const string ACC_EXTENSION=".acc";
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
	ofstream outfA,outfT;
	char raw,rawDta[4];
	short readingData,index,dtaSetSize;
	unsigned int timeStamp,clumpIndex;
	Word a;
	if(argc<2)
		cout<<"No path found"<<endl;
	else{		
		inpf.open(argv[1],ifstream::binary);
		string outFileName(argv[1]);
		if(inpf.is_open()){
			outfA.open((outFileName+ACC_EXTENSION).c_str(),ofstream::out);
			outfT.open((outFileName+TEMP_EXTENSION).c_str(),ofstream::out);
			inpf.read(&raw,1);
			dtaSetSize=short(raw);
			index=0;
			clumpIndex=0;
			while(!inpf.eof()){
				switch(index%NUMBER_OF_DATA_FIELD){
					case 0:
					inpf.read(rawDta,4);
					timeStamp=combineDoubleWord(rawDta);
					outfA<<clumpIndex<<',';
					outfA<<timeStamp<<',';
					break;
					case 1:
					case 2:
					inpf.read(rawDta,2);
					readingData=combineShort(rawDta);
					outfA<<decodeAcc(readingData,SENSIVITY)<<',';
					break;
					case 3:
					inpf.read(rawDta,2);
					readingData=combineShort(rawDta);
					outfA<<decodeAcc(readingData,SENSIVITY)<<endl;
					default:
					break;
				}
				if((++index)==dtaSetSize*NUMBER_OF_DATA_FIELD){
					index=0;
					clumpIndex++;
					if(!inpf.eof()){
						inpf.read(rawDta,2);
						readingData=combineShort(rawDta);
						outfT<<timeStamp<<','<<decodeTemp(readingData)<<endl;
					}
				}
			}
			inpf.close();
			outfA.close();
			outfT.close();
		}else
			cout<<"There is no file according to the given path"<<endl;
	}
	return 0;
}
