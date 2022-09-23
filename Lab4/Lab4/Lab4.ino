#include <Arduino_FreeRTOS.h>
#include "ESP8266.h"

#define SSID        "yin"
#define PASSWORD    "0988140518"
#define HOST_NAME   "192.168.137.1"
#define HOST_PORT   (16619)
# define LED 5

SoftwareSerial mySerial(8, 9);
ESP8266 wifi(mySerial);
TaskHandle_t xHandle;

char led_blink = '0';

void TaskBlink(void *pvParameters){
  vTaskSuspend(xHandle);
  
  for(;;){
    if(led_blink == '1'){
      digitalWrite(LED, HIGH);
      vTaskDelay(500/portTICK_PERIOD_MS);
    }
    digitalWrite(LED, LOW);
    vTaskDelay(500/portTICK_PERIOD_MS);
  }
  
}

void TaskWifi(void* pvParameters){

  Serial.print("FW Version:");
  Serial.println(wifi.getVersion().c_str());
  
  if (wifi.setOprToStationSoftAP()) {
      Serial.print("to station + softap ok\r\n");
  } else {
      Serial.print("to station + softap err\r\n");
  }

  if (wifi.joinAP(SSID, PASSWORD)) {
      Serial.print("Join AP success\r\n");
      Serial.print("IP:");
      Serial.println(wifi.getLocalIP().c_str());       
  } else {
      Serial.print("Join AP failure\r\n");
  }
  
  if (wifi.disableMUX()) {
      Serial.print("single ok\r\n");
  } else {
      Serial.print("single err\r\n");
  }
  
  Serial.print("setup end\r\n");
    
  uint8_t buffer[8] = {0};
  
  if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
      Serial.print("create tcp ok\r\n");
  } else {
      Serial.print("create tcp err\r\n");
  }
  
  vTaskResume(xHandle);
  
  for(;;){
    String prVal = String(int(analogRead(A2)));
    
    char *pr = prVal.c_str();
    wifi.send((const uint8_t*)pr, strlen(pr));
    
    uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
    if (len > 0) {
        Serial.print("Received:[");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
            led_blink = buffer[i];             
        }
        Serial.print("]\r\n");
    }  
  }
  
  if (wifi.releaseTCP()) {
      Serial.print("release tcp ok\r\n");
  } else {
      Serial.print("release tcp err\r\n");
  }

  vTaskDelay(500/portTICK_PERIOD_MS);
  
}

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  
  /* Now set up two tasks to run independently */
  /*xTaskCreate( target, name, stack size, parameter_pointer, priority, task_handle);*/
  xTaskCreate(TaskWifi, "Wifi", 256, NULL, 1, NULL);
  xTaskCreate(TaskBlink, "Blink", 64, NULL, 1, &xHandle);
}

void loop() {
  // put your main code here, to run repeatedly:

}
