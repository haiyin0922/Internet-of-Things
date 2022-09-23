#include <Wire.h>

int msg;
void setup() {
  Wire.begin();
  Serial.begin(9600);
}

void loop() {
    Wire.requestFrom(2, 1);
    
    while(Wire.available()){
      msg = Wire.read();
      Serial.println(msg);
    }
    if(msg < 150){
      Wire.beginTransmission(2);
      Wire.write(1);
      Wire.endTransmission(); 
    }
    else{
      Wire.beginTransmission(2);
      Wire.write(0);
      Wire.endTransmission();
    }
}
