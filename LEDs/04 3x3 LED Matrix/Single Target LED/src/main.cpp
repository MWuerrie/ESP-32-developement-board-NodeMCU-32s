#include <Arduino.h>

// ============================================
// 3x3 LED Matrix
// ============================================



// ############################################
// ****** CONFIGURATION ******
// ############################################

// 1. Configuration of rows and columns GPIOs
//              ROW  1   2   3 (Indexing: 0,1,2)
const int ROWS[] = {27, 26, 25};

//              COL  1   2   3 (Indexing: 0,1,2)
const int COLS[] = {23, 22, 19};
const int size_of_Matrix = sizeof(ROWS) / sizeof(ROWS[0]); // only for squared Matrix


// 2. Define target LED
// Example: Row 1, Column 2 -> GPIO 27 and GPIO 22
const int TARGET_ROW_INDEX = 0;
const int TARGET_COL_INDEX = 1;

// ############################################
// ****** INITIAL SETUP ******
// ############################################

void setup() {
// Set all row GPIOs as OUTPUT and LOW
// All LEDs are switched off at the beginning
for (int i = 0; i < size_of_Matrix; i++) {
pinMode(ROWS[i], OUTPUT);
digitalWrite(ROWS[i], LOW);
}

// Set all column GPIOs as OUTPUT and HIGH
// HIGH means that the columns are switched off
for (int i = 0; i < size_of_Matrix; i++) {
pinMode(COLS[i], OUTPUT);
digitalWrite(COLS[i], HIGH);
}
}

// ############################################
// ****** LOOP ( Main program ) ******
// ############################################
void loop() {

// 1. BLANKING: Switch all LEDs completely off
// This helps to prevent unwanted LEDs from glowing
for (int i = 0; i < size_of_Matrix; i++) {
digitalWrite(ROWS[i], LOW);
digitalWrite(COLS[i], HIGH);
}

// Give the GPIOs a very short time to electrically stabilize
delayMicroseconds(10);

// 2. SWITCH ON THE TARGET LED
// Example: Row 0, Column 1
digitalWrite(ROWS[TARGET_ROW_INDEX], HIGH);
digitalWrite(COLS[TARGET_COL_INDEX], LOW);

// Give the LED enough time to be visible
delay(5);
}




