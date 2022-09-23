#include <Wire.h>

const int photocellPin = A2, ledPin = 9;

void setup() {
  Serial.begin(9600);
  Wire.begin(2);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  pinMode(ledPin, OUTPUT);
}

void loop() {
}

void requestEvent() {
  int photocellVal = analogRead(photocellPin);
  Wire.write(photocellVal);
}

void receiveEvent(int num) {
  digitalWrite(ledPin, Wire.read());
}
