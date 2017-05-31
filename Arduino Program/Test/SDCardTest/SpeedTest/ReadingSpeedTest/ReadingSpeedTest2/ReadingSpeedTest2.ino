#include <SD.h>
#define MAX_TEST_BUFF 500
#define RESOLUTION 10  //jumping step
void setup(){
  char buf[MAX_TEST_BUFF];  //this is just for fun
  SD.begin(4);
  Serial.begin(9600);
  File f=SD.open("1");
  unsigned long en,beg,fileSize=f.size();
  f.close();
  Serial.print("File Size:");
  Serial.println(fileSize);
  for(int i=0;i<=MAX_TEST_BUFF;i+=RESOLUTION){
    if(!i) continue;
    f=SD.open("1");
    beg=micros();
    while(f.available()){
      f.read(buf,i);
    }
    en=micros();
    f.close();
    Serial.print(i);
    Serial.print("  ");
    Serial.println(fileSize/(float(en-beg)/1000000));
  }
}
void loop(){
}
