const int  buttonPin = 2, resistorPin = A0;
const int r1_ledPin = 13, r2_ledPin = 12, g_ledPin = 11, y_ledPin = 10;
int ledState = LOW, buttonState = LOW, lastButtonState = LOW;
int resistorValue = 0, mode = 1, led_high = 13, led_low = 10;
unsigned long lastDebounceTime = 0, debounceDelay = 50;

void setup() {
  pinMode(r1_ledPin, OUTPUT);
  pinMode(r2_ledPin, OUTPUT);
  pinMode(g_ledPin, OUTPUT);
  pinMode(y_ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
}

void loop() {
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if(reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        mode = !mode;
      }
    }
  }
  lastButtonState = reading;
  
  resistorValue = analogRead(resistorPin);
  if (mode) {
    digitalWrite(r1_ledPin, ledState);
    digitalWrite(r2_ledPin, ledState);
    digitalWrite(g_ledPin, ledState);
    digitalWrite(y_ledPin, ledState);
    delay(resistorValue);
    ledState = !ledState;
  }
  else {
    digitalWrite(led_low, LOW);
    digitalWrite(led_high, HIGH);
    delay(resistorValue);
    led_high = (led_high == 10)? 13 : led_high-1;
    led_low =  (led_low == 10)? 13 : led_low-1;   
  }
}
