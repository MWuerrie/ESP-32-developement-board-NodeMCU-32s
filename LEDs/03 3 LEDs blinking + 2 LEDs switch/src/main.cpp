
// """ Here we want to provide two different tasks with the ESP 32.
//     In total we have five LEDs.
//     First task  (A) : Three LEDs flashing in sequence
//     Second task (B) : Using a button to switch between two extra LEDs"""  


#include <Arduino.h>

// ############################################
//        ****** CONFIGURATION ******
// ############################################


// ============================================
// (A): Flashing sequence LED 1,2,3
// ============================================

//    1.  Configuration of LEDs and GPIOs
const int ledPins[] = {25, 26, 27};     // 
const int number_of_Leds = sizeof(ledPins) / sizeof(ledPins[0]);  // number of LEDs we use

//    2.  Timing variables
unsigned long timer = 0;
unsigned long runtime_interval = 500; // e.g.: 300 ms

//    3. Define Index of LED
int LED_index = 0;


// ==========================================
// (B):  Switchbutton and LEDs 4,5
// ==========================================

// 1. Assignment of the GPIOs:
const int buttonPin = 32;
const int led_4 = 23;
const int led_5 = 22;

// Status variables for the switch button
bool button_old_state = HIGH; 
bool state_Led_A = true;     // Startzustand: LED A ist aktiv, LED B aus




// ############################################
//        ****** INITIAL SETUP ******
// ############################################

void setup() {
// ============================================
// (A): Flashing sequence LED 1,2,3
// ============================================


  //     Set all GPIOs Pins as Output and on LOW
  //     All LEDs will be out in the beginning
    for (int i = 0; i < number_of_Leds;i++){
        pinMode(ledPins[i], OUTPUT);
        digitalWrite(ledPins[i], LOW);
    } 

// ============================================
// (B): Switchbutton and LEDs 4,5
// ============================================

  // INPUT_PULLUP activates intern resistor of the ESP32 and set the buttonPin on HIGH. 
  // (by default on HIGH (3,3V) and by pressing on LOW (GND).
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(led_4, OUTPUT);
  pinMode(led_5, OUTPUT);

  // Set Initial States of LED 4 and LED 5
  digitalWrite(led_4, HIGH);
  digitalWrite(led_5, LOW);
  }


// ############################################
//        ****** LOOP ( Main program ) ******
// ############################################

void loop(){

// ============================================
// (A): Flashing sequence LED 1,2,3
// ============================================

    // asking for the time in our runtime_interval
    unsigned long actual_time = millis();

    // check if our runtime_interval is over 
    if (actual_time - timer>= runtime_interval){
        timer = actual_time;  // change timer to the actual time

        // turn off the first LED
        digitalWrite(ledPins[LED_index], LOW);

        // change to next LED-index
        LED_index++;

        // if last LED, start again from the first one
        if (LED_index >= number_of_Leds)
        {
            LED_index = 0;
        }

        // Turn on the next LED
        digitalWrite(ledPins[LED_index], HIGH);
    }


    
// ============================================
// (B): Switchbutton and LEDs 4,5  
// ============================================

// !!! This is why we use millis() for the flashing sequence (A) instead of delay(),  
// !!! Now we can run this code separated from task (A)

  bool button_state_actual = digitalRead(buttonPin);

  // "Edge detection": check, if button JUST was pressed
  // (change from HIGH to LOW)
  if (button_state_actual == LOW && button_old_state == HIGH) {
    
    // Reverse the state (Toogle)
    state_Led_A = !state_Led_A; 

    // switch state of LEDs 
    if (state_Led_A) {
      digitalWrite(led_4, HIGH);
      digitalWrite(led_5, LOW);
    } else {
      digitalWrite(led_4, LOW);
      digitalWrite(led_5, HIGH);
    }

    // minimal delay, so that the fast loop of the ESP32 doesn´t interpret one single push on the button 
    // as many pushes ("Debouncing")
    delay(5); 
  }

  // save the actual state of the button for the next round
  button_old_state = button_state_actual;


}