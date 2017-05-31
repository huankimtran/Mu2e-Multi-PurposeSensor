#include "Wire.h"
#include "./utility/twi.h"
#include "MPU6050Huan.h"
MPU6050 chip;
void setup(){
  Serial.begin(9600);
  Serial.println("Running");
}
void loop(){
  Serial.println(*(chip.readData(PWR_MGMT_1,1)),BIN);
//  Serial.println(chip.readAccelX());
  delay(1000);
}
