#include <SD.h>
#define BUFF 10
void setup(){
  char buf[BUFF];
  SD.begin(4);
  File f=SD.open("1");
  unsigned long fileSize=f.size();
  unsigned long beg=micros();
  while(f.available()){
    f.read(buf,BUFF);
  }
  unsigned long en=micros();
  f.close();
  unsigned long cl=micros();
  Serial.begin(9600);
  Serial.print("Read time:");
  Serial.println(en-beg);
  Serial.print("End time:");
  Serial.println(cl-en);
  Serial.print("File size:");
  Serial.println(fileSize);
  Serial.print("Speed:");
  Serial.print(fileSize/(float(en-beg)/1000000));
  Serial.println(" byte per second");
}
void loop(){
}
