#include <Arduino.h>

// Define the GPIOs on the ESP32, in this example GPIOs 25,26 and 27:
const int led1 = 25;
const int led2 = 26;
const int led3 = 27;

void setup() {
  // Configure GPIOs as Output
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
}



void loop() {
//  Turn on and off the LEDs 

  digitalWrite(led1, HIGH); // LED 1 on --> GPIO is set on 3,3 V (HIGH)
  delay(500); // wait 500 ms
  digitalWrite(led1, LOW); // LED 1 off --> GIPO is set on 0 Volt (LOW)
  
  // LED 2 an
  digitalWrite(led2, HIGH); // LED 1 on --> GPIO is set on 3,3 V (HIGH)
  delay(500); // wait 500 ms
  digitalWrite(led2, LOW); // LED 1 off --> GIPO is set on 0 Volt (LOW)
  
  // LED 3 an
  digitalWrite(led3, HIGH); // LED 1 on --> GPIO is set on 3,3 V (HIGH)
  delay(500); // wait 500 ms
  digitalWrite(led3, LOW); // LED 1 off --> GIPO is set on 0 Volt (LOW)
}


