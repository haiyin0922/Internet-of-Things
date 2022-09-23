#include <SoftwareSerial.h>
#include <Servo.h>

SoftwareSerial MAX485(10, 11);
Servo myservo;
char val;
int DE_RE = 9;
int exPos = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("MAX485 is ready!");
  MAX485.begin(9600);
  pinMode(DE_RE,OUTPUT);
  digitalWrite(DE_RE,LOW);
  myservo.attach(12);
}

void loop() {
  int pos = 0;
  int flag = 0;
  digitalWrite(DE_RE,LOW);
  while(MAX485.available()) {
    flag = 1;
    val = MAX485.read();
    if (val == 'x') break;
    pos = pos * 10 + (val - '0');
    delay(10);
  }
    
  if(flag && (pos - exPos > 3 || exPos - pos > 3)){
    Serial.println(pos);
    exPos = pos;
    pos = map(pos, 0, 1023, 0, 180);
    myservo.write(pos);
  }

  delay(10);
}
