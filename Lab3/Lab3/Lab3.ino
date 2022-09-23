#include <SoftwareSerial.h>
#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT22 

SoftwareSerial AT(8, 9);
DHT dht(DHTPIN, DHTTYPE);
char val;
const int ledPin = 10;
int ledState = 0;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  AT.begin(9600);
  dht.begin();
  
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

  float h = dht.readHumidity();
  String combine = String(h) + "," + ledState;
  combine = "AT+QISENDEX=0,7," + toHEX(combine);
  const char *result = combine.c_str();
  command(result, 5000);
  String payload = command("AT+QIRD=0,512", 5000);
  
  decodePayload(payload);
  digitalWrite(ledPin, ledState);
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
        ledState = char(byte(ascii) - '0');
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
}
