#include <SD.h>
#define FILE_SIZE 102400  //0.5mb
#define MAX_TEST_BUFF 500
#define RESOLUTION 10  //jumping step
void setup(){
  char buf[MAX_TEST_BUFF];  //this is just for fun
  SD.begin(4);
  Serial.begin(9600);
  File f;
  unsigned long en,beg;
  Serial.print("File Size:");
  Serial.println(FILE_SIZE);
  for(int i=0;i<=MAX_TEST_BUFF;i+=RESOLUTION){
    if(!i) continue;
    f=SD.open("test.txt",FILE_WRITE);
    f.seek(0);
    beg=micros();
    for(int j=0;j<FILE_SIZE/i;j++){
      f.write((const uint8_t*)buf,i);
    }
    en=micros();
    f.close();
    Serial.print(i);
    Serial.print("  ");
    Serial.println(FILE_SIZE/(float(en-beg)/1000000));
    }
}
void loop(){
}
