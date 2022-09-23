#include <SoftwareSerial.h>
#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT22 

SoftwareSerial AT(8, 9);
DHT dht(DHTPIN, DHTTYPE);
char val;

void setup() {
  Serial.begin(9600);
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
  command("AT+QIOPEN=1,0,\"TCP\",140.114.89.68,16542,0,0,0", 5000);

  String id = "105072123,";
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  id = id + String(t,1) + "," + String(h,1);
  id = "AT+QISENDEX=0,19," + toHEX(id);
  const char *result = id.c_str();
  command(result, 5000);
  command("AT+QIRD=0,512", 5000);
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
      result += c;  // append to the result string
    }
  }
  Serial.println();  // new line after timeout.
  return result;
}

String toHEX(String toSend) {
  String result = "";
  for(int i=0; i<toSend.length(); i++){
    result += String(int(toSend[i]), HEX);
  }
  return result;
}

void loop() {
}
