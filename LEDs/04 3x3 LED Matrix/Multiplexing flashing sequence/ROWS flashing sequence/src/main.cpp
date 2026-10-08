#include <Arduino.h>

// ============================================
// Configuration
// ============================================

// LED matrix GPIO configuration
constexpr uint8_t ROW_PINS[] = {27, 26, 25};
constexpr uint8_t COL_PINS[] = {23, 22, 19};

constexpr size_t MATRIX_SIZE =
sizeof(ROW_PINS) / sizeof(ROW_PINS[0]);

// Time between two row changes.
// This determines the speed of the visible sequence.
constexpr unsigned long ROW_INTERVAL_MS = 500;

// Multiplexing interval.
// The active row is refreshed at this frequency.
constexpr unsigned long MULTIPLEX_INTERVAL_US = 1000;

// ============================================
// Global Variables
// ============================================

// Currently active row.
uint8_t activeRow = 0;

// Timestamp for the row sequence.
unsigned long lastRowChange = 0;

// Timestamp for the multiplexing refresh.
unsigned long lastMultiplex = 0;

// ============================================
// LED Matrix Functions
// ============================================

// Switch all LEDs off.
void clearMatrix()
{
for (size_t i = 0; i < MATRIX_SIZE; ++i)
{
digitalWrite(ROW_PINS[i], LOW);
digitalWrite(COL_PINS[i], HIGH);
}
}

// Activate all LEDs in the selected row.
//
// One row is switched HIGH while all columns
// are switched LOW.
void activateRow(uint8_t row)
{
clearMatrix();

// Short blanking interval to prevent ghosting.
delayMicroseconds(10);

digitalWrite(ROW_PINS[row], HIGH);

for (size_t col = 0; col < MATRIX_SIZE; ++col)
{
    digitalWrite(COL_PINS[col], LOW);
}

}

// ============================================
// Matrix Initialization
// ============================================

void initializeMatrix()
{
// Initialize row pins.
for (size_t i = 0; i < MATRIX_SIZE; ++i)
{
pinMode(ROW_PINS[i], OUTPUT);
digitalWrite(ROW_PINS[i], LOW);
}

// Initialize column pins.
for (size_t i = 0; i < MATRIX_SIZE; ++i)
{
    pinMode(COL_PINS[i], OUTPUT);
    digitalWrite(COL_PINS[i], HIGH);
}

clearMatrix();

}

// ============================================
// Multiplexing
// ============================================

// Refresh the currently active row.
//
// The row is refreshed very frequently so that
// the LED matrix remains visually stable.
void updateMultiplexing()
{
const unsigned long currentTime = micros();

if (currentTime - lastMultiplex >= MULTIPLEX_INTERVAL_US)
{
    lastMultiplex = currentTime;

    activateRow(activeRow);
}

}

// ============================================
// Row Sequence
// ============================================

// Change to the next row after the defined interval.
void updateRowSequence()
{
const unsigned long currentTime = millis();

if (currentTime - lastRowChange >= ROW_INTERVAL_MS)
{
    lastRowChange = currentTime;

    ++activeRow;

    // Restart with the first row
    // after the last row.
    if (activeRow >= MATRIX_SIZE)
    {
        activeRow = 0;
    }
}

}

// ============================================
// Setup
// ============================================

void setup()
{
Serial.begin(115200);

initializeMatrix();

}

// ============================================
// Main Loop
// ============================================

void loop()
{
// Refresh the active row continuously.
updateMultiplexing();

// Change the active row at a slower interval.
updateRowSequence();

}