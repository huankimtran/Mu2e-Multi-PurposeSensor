#include <AM2302.h>
#include <LiquidCrystal.h>
AM2302 sensor(11);
byte* dta;
LiquidCrystal lcd(10, 9, 8, 0, 1, 2, 3, 4, 5, 6,7);
void setup(){
    lcd.begin(16,2);
}
void loop() {
  lcd.println("1");
  if(sensor.read(dta)==READ_OK)
    lcd.println("OK");    
  else
    lcd.println("Not good");
  lcd.println("2");
  delay(2200);
}
