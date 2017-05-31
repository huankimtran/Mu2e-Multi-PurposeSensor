#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include <SD.h>
#include <DHT.h>
/*---------------USED-PIN-----------------
 * 10 11 12 13 - SPI connection
 * A4(SDA) A5(SCL) - I2c
 * 4 : SDCard
 * 5 : Mode selector
 * 8 : Mode indicator
 * 2 : INT MPU650
 */
//----------------DHT22-Temperature-humidity-CONFIGURATION------
#define DHTPIN 3
#define DHTTYPE DHT22
class TEMPSENSOR:public DHT{
  public:
  void initialize(){
    begin();
    lastRun=0;
  }
  void recordTemp(){
  }
  private:
  unsigned long lastRun;
  
};
//----------------ACCELERATION-SENSOR-CONFIGURATION-----

/*----------------NOTE-ON-BUFFERR-SIZE-----------------
 *                                                     *
 *                 Don't Ever Set Buffer_Size          *                         
 *                 THAN 30                             *
 *                  70 still run but no data recorded  *
 *-----------------------------------------------------*/
#define BUFFER_SIZE 20

#define INT_PIN 2
#define INT_MODE 1 //Active low
#define INT_LATCH_MODE 1 //latch-until-int-cleared
#define INT_LATCH_CLEAR_MODE 1 //Any Register read
#define ACCEL_SCALE MPU6050_ACCEL_FS_8 //+-8 scale
#define SAMPLING_FREQ 31  //This set the sampling rate to 0.25kHz
struct accel_data{
  int16_t ax,ay,az;
  unsigned long int timeStamp;
};
class ACCELSENSOR:public MPU6050{
  public:
  uint8_t index;
  accel_data buffer[BUFFER_SIZE];
  void initializeAccel(){
    index=0;
    //Setting up connection with the MPU6050
    Wire.begin();  
    initialize();  
    setFullScaleAccelRange(ACCEL_SCALE); //Sacle is now +-8g
    setRate(SAMPLING_FREQ);              //Sampling freq to 0.25Khz
    setInterruptMode(INT_MODE);                //Interrupt Active-low mode
    setInterruptLatch(INT_LATCH_MODE);               //Until clear
    setInterruptLatchClear(INT_LATCH_CLEAR_MODE);           // Clear when read
    setIntDataReadyEnabled(1);               //Interrupt to inform MCU
    pinMode(INT_PIN,INPUT_PULLUP);
  }
};
//-------------------SD-CARD-CONFIGURATION---------------
/*
 *the META file contains 
 *the number of data files existing in the Card
 *Remeber to keep file name length under 8 characters
 */
/*
 *Data file structure:
 *First byte: indicate the buffer size ( say N)
 *A set of data includes 80 bits in which
 *First 32 bit - Time stamp 
 *1st 16 bits Field - raw X data
 *2nd 16 bits Field - raw Y data
 *3rd 16 bits Field - raw Z data
 *The data set Nth will an extra field 
 *the 4th 16 bits Field - raw Temperature data
 */
//------Configuration-------------
#define SD_CS 4
#define OPERATING_MODE_PIN 5 
#define MODE_INDICATOR_PIN 8
#define MAX_FILE_NAME_LENGTH 8
#define READING_MODE true
#define SERIAL_BAUD 460800  //Worked fine at 115200,9600,460800
//------Commands----------------
#define ARDUINO_SALUTE 0
#define ARDUINO_PULL 1
#define ARDUINO_ANS 254
//------Transmission-command-----
#define START_HEADING 1
#define START_TEXT  2 
#define END_TEXT 3
#define END_OF_TRANS 4
//-----------Class-Definition-----------
class SDCARD{
  public: 
    char* META="META";
    char AccdataFileName[MAX_FILE_NAME_LENGTH];  
    File meta,data;
    int fileIndex;
    boolean lastMode;
    //-----------FOR-LOGGING-------------------------
    void initializeSD2Log(){
      if(Serial)
        Serial.end();
      pinMode(SD_CS,OUTPUT);
      SD.begin(SD_CS);
      if(!SD.exists(META)){
      //No META file exist, create one and set the value to 1
      //0 is reserved for no file signal
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
      String(fileIndex).toCharArray(AccdataFileName,MAX_FILE_NAME_LENGTH);
      data=SD.open(AccdataFileName,FILE_WRITE);
      data.write(BUFFER_SIZE);
      data.close();
    }
    File getAccDataFile(){
      return SD.open(AccdataFileName,FILE_WRITE);
    }
    static uint8_t getLowByte(uint16_t dta){
      return dta&0xff;
    }
    static uint8_t getHighByte(uint16_t dta){
      return (dta>>8)&0xff;
    }
    void writeAWord(File file,int16_t dta){
      file.write(getHighByte(dta));
      file.write(getLowByte(dta));
      file.flush();
    }
    void writeDoubleWord(File file,unsigned long int dta){
      uint8_t byteHH,byteHL,byteLH,byteLL;
      byteHH=byteHL=byteLH=byteLL=0xff;
      byteLL&=dta;
      dta=dta>>8;
      byteLH&=dta;
      dta=dta>>8;
      byteHL&=dta;
      dta=dta>>8;
      byteHH&=dta;
      file.write(byteHH);
      file.write(byteHL);
      file.write(byteLH);
      file.write(byteLL);
      file.flush();
    }
    void writeSetsOfAccData(accel_data dta[BUFFER_SIZE],int length,uint16_t temp){
      File file=getAccDataFile();
      for(int i=0;i<length;i++){
        accel_data tmp=*(dta+i);
        writeDoubleWord(file,tmp.timeStamp);
        writeAWord(file,tmp.ax);
        writeAWord(file,tmp.ay);
        writeAWord(file,tmp.az);
      }
      file.close();
    }
//----------FOR-READING-(PULLING)-------------
    int initializeSD2Read(){
      pinMode(MODE_INDICATOR_PIN,OUTPUT);
      pinMode(OPERATING_MODE_PIN,INPUT);
      digitalWrite(MODE_INDICATOR_PIN,READING_MODE);
      lastMode=READING_MODE;
      if(Serial)
        Serial.begin(SERIAL_BAUD);
      pinMode(SD_CS,OUTPUT);
      SD.begin(SD_CS);
      if(!SD.exists(META))
        return 0;
      else{
      //If there exists a meta file, read the value
        meta=SD.open(META,FILE_READ);
        fileIndex=meta.read();
        meta.close();
      }
      return fileIndex;
    }
    void sendFile(int fName){
      File tmpF=SD.open(Int2Char(fName));
      unsigned long tmpFS=tmpF.size();
      Serial.write(START_HEADING);
      Serial.print(tmpFS);
      Serial.write(END_TEXT);
      while(tmpF.available())
        Serial.write(tmpF.read());
      tmpF.close();
    }
    char* Int2Char(int fName){
      String(fName).toCharArray(AccdataFileName,MAX_FILE_NAME_LENGTH);
      return AccdataFileName;
    }
    void onSalute(){
      Serial.write(ARDUINO_ANS);
    }
    void onPullFile(){
      //Send number of file
      Serial.write(fileIndex);
      for(int i=1;i<=fileIndex;i++)
        sendFile(i);
    }
};
//--------------------Program---------------------
SDCARD card;
ACCELSENSOR acelSen;
void setup(){
  pinMode(OPERATING_MODE_PIN,INPUT);
  if(digitalRead(OPERATING_MODE_PIN)==READING_MODE)
    card.initializeSD2Read();
  else{
      acelSen.initializeAccel();
      card.initializeSD2Log();
  }
}
void loop(){
  digitalWrite(MODE_INDICATOR_PIN,digitalRead(OPERATING_MODE_PIN));
  if(card.lastMode!=digitalRead(OPERATING_MODE_PIN))
    setup();
  if(digitalRead(OPERATING_MODE_PIN)==READING_MODE){
    if(Serial.available())
      switch(Serial.read()){
        case ARDUINO_SALUTE:
        card.onSalute();
        break;
        case ARDUINO_PULL:
        card.onPullFile();
        break;
        }
  }else{
    if(!digitalRead(INT_PIN)){
      //Getting new data
      acelSen.getAcceleration(&(acelSen.buffer[acelSen.index].ax),&(acelSen.buffer[acelSen.index].ay),&(acelSen.buffer[acelSen.index].az));
      acelSen.buffer[acelSen.index].timeStamp=millis();
      //Clear the interrupt status- just for sure
      acelSen.getIntStatus();
      if((++acelSen.index)==BUFFER_SIZE){
        //Restart the buffer
        acelSen.index=0;
        //SAVE DATA TO CARD
        card.writeSetsOfAccData(acelSen.buffer,BUFFER_SIZE,acelSen.getTemperature());
      }
    }
  }
}
