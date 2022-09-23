#include <SoftwareSerial.h>

SoftwareSerial AT(8, 9);
char val;
const int ringPin = 7, xpin = A1, ypin = A2, zpin = A3;
int x, y, z, x_prev, y_prev, z_prev;

void setup() {
  Serial.begin(9600);
  AT.begin(9600);
  pinMode(ringPin, OUTPUT);
  
  command("AT+CSQ", 2000);
  command("AT+QSCLK=0", 2000);
  command("AT+QBAND=1,8", 2000);
  command("AT+CFUN=1", 2000);
  command("AT+CGSN=1", 2000);
  command("AT+CIMI", 2000);
  command("AT+CGATT=0", 2000);
  command("AT+CGDCONT=1,\"IPV4V6\"", 2000);
  command("AT+CGATT=1", 5000);
  command("AT+QICFG=\"dataformat\",1,1", 5000);
  command("AT+QIOPEN=1,0,\"TCP\",140.114.78.132,16619,0,0,0", 5000);

  x_prev = analogRead(xpin);
  y_prev = analogRead(ypin);
  z_prev = analogRead(zpin);
}

String command(const char *toSend, unsigned long milliseconds) {
  String result = "";
  Serial.print("Sending: ");
  Serial.println(toSend);
  AT.println(toSend);
  unsigned long startTime = millis();
  Serial.print("Received: ");
  while (millis() - startTime < milliseconds) {
    if (AT.available()) {
      char c = AT.read();
      Serial.write(c);
      result += c;
    }
  }
  Serial.println();
  return result;
}

String toHEX(String toSend) {
  String result = "";
  for(int i=0; i<toSend.length(); i++){
    result += String(int(toSend[i]), HEX);
  }
  return result;
}

void toString(String hex) {
  int ascii = 0;
  for(int i=0; i<hex.length(); i++){    
    if(i%2 == 0){
      if(i > 0)
        Serial.print(char(byte(ascii)));
      ascii = 0;
    }
    else
      ascii *= 16;
      
    if ((hex[i]>='0') && (hex[i]<='9'))
      ascii +=  hex[i]-'0';
    if ((hex[i]>='A') && (hex[i]<='F'))
      ascii += hex[i]+10-'A' ;
  }
  
  return;
}

void decodePayload(String str) {
  int cnt = 0;
  String result = "";
  for(int i=0; i<str.length(); i++){
    if(cnt==2)
      result += str[i];
    if(cnt==3)
      break;
    if(str[i] == '\n')
      cnt++;
  }
  Serial.print("Payload: " + result);
  toString(result);
  
  return;
}

void loop() {
  x = analogRead(xpin);
  y = analogRead(ypin);
  z = analogRead(zpin);
  
  if(abs(x-x_prev)>5 || abs(y-y_prev)>5 || abs(z-z_prev)>5){
    for(int i = 0; i < 5; i++){
      for(int hz = 650; hz < 1450; hz++){
        tone(ringPin, hz, 50);
        delay(4);
      }
      noTone(ringPin);
   
      for(int hz = 1450; hz > 650; hz--){
        tone(ringPin, hz, 50);
        delay(2);
      }
      noTone(ringPin);
    }
    
    command("AT+QISENDEX=0,1,31", 5000);
    String payload = command("AT+QIRD=0,512", 5000);
    decodePayload(payload);
  }
  
  else{
    command("AT+QISENDEX=0,1,30", 5000);
    String payload = command("AT+QIRD=0,512", 5000);
    decodePayload(payload);  
  }

  x_prev = x;
  y_prev = y;
  z_prev = z;
}
