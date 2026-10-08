#include <Arduino.h>

// ============================================
// Configuration
// ============================================

// LED matrix GPIO configuration
constexpr uint8_t ROW_PINS[] = {27, 26, 25};
constexpr uint8_t COL_PINS[] = {23, 22, 19};

constexpr size_t MATRIX_SIZE =
sizeof(ROW_PINS) / sizeof(ROW_PINS[0]);

// Multiplexing interval
// Determines how quickly the active row is refreshed.
constexpr unsigned long MULTIPLEX_INTERVAL_US = 1000;

// Sequence interval
// Determines how long each LED remains selected.
constexpr unsigned long SEQUENCE_INTERVAL_MS = 500;

// ============================================
// Global Variables
// ============================================

// Currently selected LED in the sequence.
uint8_t sequenceIndex = 0;

// Timestamp for multiplexing.
unsigned long lastMultiplexTime = 0;

// Timestamp for the LED sequence.
unsigned long lastSequenceTime = 0;

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

// Activate a single LED.
//
// The LED is addressed using a row and column index.
void activateLED(uint8_t row, uint8_t col)
{
clearMatrix();

// Short blanking interval to prevent ghosting.
delayMicroseconds(10);

digitalWrite(ROW_PINS[row], HIGH);
digitalWrite(COL_PINS[col], LOW);

}

// ============================================
// Matrix Initialization
// ============================================

void initializeMatrix()
{
// Initialize all row pins.
for (size_t i = 0; i < MATRIX_SIZE; ++i)
{
pinMode(ROW_PINS[i], OUTPUT);
digitalWrite(ROW_PINS[i], LOW);
}

// Initialize all column pins.
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

// Refresh the currently selected LED.
//
// In this simple 3x3 test, only one LED is active
// at a time. The frequent refresh keeps the LED
// continuously visible.
void updateMultiplexing()
{
const unsigned long currentTime = micros();

if (currentTime - lastMultiplexTime >=
    MULTIPLEX_INTERVAL_US)
{
    lastMultiplexTime = currentTime;

    const uint8_t row = sequenceIndex / MATRIX_SIZE;
    const uint8_t col = sequenceIndex % MATRIX_SIZE;

    activateLED(row, col);
}

}

// ============================================
// LED Sequence
// ============================================

// Select the next LED in the sequence.
void updateSequence()
{
const unsigned long currentTime = millis();

if (currentTime - lastSequenceTime >=
    SEQUENCE_INTERVAL_MS)
{
    lastSequenceTime = currentTime;

    ++sequenceIndex;

    // Restart the sequence after the last LED.
    if (sequenceIndex >= MATRIX_SIZE * MATRIX_SIZE)
    {
        sequenceIndex = 0;
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
// Update the selected LED at high frequency.
updateMultiplexing();

// Change the selected LED at a slower interval.
updateSequence();

}