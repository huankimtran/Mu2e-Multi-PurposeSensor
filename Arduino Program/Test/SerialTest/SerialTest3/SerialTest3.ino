//This is used with the COMM/Testing/SpeedTest1.cpp folder
#define BAUD_RATE 921600
#define KEY 100
#define HELLO 101
#define MESSAGE_SIZE 1024000
#define USING_BUFF 100
#define MAX_BUFF 500
uint8_t buf[MAX_BUFF];
void setup(){
  for(int i=0;i<MAX_BUFF;i++)
    buf[i]=KEY;
  Serial.begin(BAUD_RATE);
}
void loop(){
  if(Serial.available()){
    switch(Serial.read()){
      case KEY:
      for(int i=0;i<MESSAGE_SIZE/USING_BUFF;i++)
        Serial.write(buf,USING_BUFF);
      Serial.write(HELLO);
      break;
      case HELLO:
      Serial.write(HELLO);
      break;
    }
  }
}
