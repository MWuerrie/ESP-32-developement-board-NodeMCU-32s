// First Testrun for blinking LED connected to an NodeMcu32 (Esp32)


#include <Arduino.h>
// GPIO 27 is supporting the voltage 
const int ledPin = 27;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);
  delay(1000);
}