#define OPERATING_MODE_PIN 4 
#define READING_MODE false
#define SENDING_PASS 89
#define RESPONSE_PASS 90
void setup(){
  pinMode(OPERATING_MODE_PIN,INPUT);
  if(digitalRead(OPERATING_MODE_PIN)==READING_MODE)
    Serial.begin(9600);  

}
void loop(){
  if(digitalRead(OPERATING_MODE_PIN)==READING_MODE){
    if(Serial.available())
      if(Serial.read()==SENDING_PASS)
        Serial.write(RESPONSE_PASS);
  }
}

