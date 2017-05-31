#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#define BUFFER_SIZE 100
#define INT_PIN 2
#define INT_MODE 1 //Active low
#define INT_LATCH_MODE 1 //latch-until-int-cleared
#define INT_LATCH_CLEAR_MODE 1 //Any Register read
#define ACCEL_SCALE MPU6050_ACCEL_FS_8 //+-8 scale
#define SAMPLING_FREQ 31  //This set the sampling rate to 0.25kHz

volatile bool readMPU;
uint8_t index=0;
MPU6050 chip;
struct accel_data{
  int16_t ax,ay,az;
  unsigned long int timeStamp;
};
accel_data buffer[BUFFER_SIZE];
void setup(){
  Serial.begin(9600);
  //Setting up connection with the MPU6050
  Serial.println(chip.getIntEnabled(),BIN);
  Wire.begin();  
  chip.initialize();
  chip.setFullScaleAccelRange(ACCEL_SCALE); //Sacle is now +-8g
  chip.setRate(SAMPLING_FREQ);              //Sampling freq to 0.5Khz
  chip.setInterruptMode(INT_MODE);                //Interrupt Active-low mode
  chip.setInterruptLatch(INT_LATCH_MODE);               //Until clear
  chip.setInterruptLatchClear(INT_LATCH_CLEAR_MODE);           // Clear when read
  chip.setIntDataReadyEnabled(1);               //Interrupt to inform MCU
  pinMode(INT_PIN,INPUT_PULLUP);
}
void loop(){
  while(!digitalRead(2)){
    chip.getAcceleration(&(buffer[index].ax),&(buffer[index].ay),&(buffer[index].az));
    buffer[index].timeStamp=millis();
    Serial.println(buffer[index].timeStamp);
    chip.getIntStatus();
    readMPU=false;
    if((++index)==BUFFER_SIZE){
      index=0;
      //ADD CODE TO SAVE DATA TO CARD HERE
    }
  }
}
