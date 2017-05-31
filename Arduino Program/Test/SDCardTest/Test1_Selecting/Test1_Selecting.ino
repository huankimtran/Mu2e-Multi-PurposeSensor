#include <SD.h>
#define SD_CS 4
/*
 *the META file contains 
 *the number of data files existing in the Card
 *Remeber to keep file name length under 8 characters
 */
char* META="META";
char dataFileName[100];
File meta,data;
int fileIndex;
void setup(){
  SD.begin(SD_CS);
  if(!SD.exists(META)){
    //No META file exist, create one and set the value to 1
    meta=SD.open(META,FILE_WRITE);
    meta.write(1);
    fileIndex=1;
  }else{
    //If there exists a meta file, read the value, increase it
    //and rewrite the value
    meta=SD.open(META,FILE_READ);
    fileIndex=meta.read();
    meta.close();
    meta=SD.open(META,FILE_WRITE);
    meta.seek(0);
    meta.write(++fileIndex);
  }
  meta.close();
  String(fileIndex).toCharArray(dataFileName,100);
  data=SD.open(dataFileName,FILE_WRITE);
  data.println("Hello world");
  data.close();
}
void loop(){}
