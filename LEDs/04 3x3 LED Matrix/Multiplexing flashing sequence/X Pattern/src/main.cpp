#include <Arduino.h>

// ============================================
// Configuration
// ============================================

// LED matrix GPIO configuration
constexpr uint8_t ROW_PINS[] = {27, 26, 25};
constexpr uint8_t COL_PINS[] = {23, 22, 19};

constexpr size_t MATRIX_SIZE =
sizeof(ROW_PINS) / sizeof(ROW_PINS[0]);

// Multiplexing interval.
// Each row is activated for a short period
// before switching to the next row.
constexpr unsigned long MULTIPLEX_INTERVAL_US = 1000;

// ============================================
// LED Pattern
// ============================================

// X pattern:
//
// LED matrix:
//
// X . X
// . X .
// X . X
//
// "true" = LED is ON
// "false" = LED is OFF
const bool X_PATTERN[MATRIX_SIZE][MATRIX_SIZE] =
{
{true, false, true},
{false, true, false},
{true, false, true}
};

// ============================================
// Global Variables
// ============================================

// Currently active row.
uint8_t activeRow = 0;

// Timestamp for the multiplexing refresh.
unsigned long lastMultiplex = 0;

// ============================================
// LED Matrix Functions
// ============================================

// Switch all LEDs off.
//
// This is used before changing the active row
// to prevent unwanted LEDs from briefly lighting up.
void clearMatrix()
{
for (size_t row = 0; row < MATRIX_SIZE; ++row)
{
digitalWrite(ROW_PINS[row], LOW);
}

for (size_t col = 0; col < MATRIX_SIZE; ++col)
{
    digitalWrite(COL_PINS[col], HIGH);
}

}

// Activate the currently selected row.
//
// Only LEDs marked as "true" in X_PATTERN
// will be switched on.
void activateCurrentRow()
{
clearMatrix();

// Short blanking interval to prevent ghosting.
delayMicroseconds(10);

// Activate the current row.
digitalWrite(ROW_PINS[activeRow], HIGH);

// Activate the required columns.
for (size_t col = 0; col < MATRIX_SIZE; ++col)
{
    if (X_PATTERN[activeRow][col])
    {
        digitalWrite(COL_PINS[col], LOW);
    }
}

}

// ============================================
// Multiplexing
// ============================================

// Refresh the matrix one row at a time.
//
// The rows are switched so quickly that the
// human eye perceives the complete X pattern.
void updateMultiplexing()
{
const unsigned long currentTime = micros();

if (currentTime - lastMultiplex >= MULTIPLEX_INTERVAL_US)
{
    lastMultiplex = currentTime;

    activateCurrentRow();

    // Move to the next row.
    ++activeRow;

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

// Initialize all row pins.
for (size_t row = 0; row < MATRIX_SIZE; ++row)
{
    pinMode(ROW_PINS[row], OUTPUT);
    digitalWrite(ROW_PINS[row], LOW);
}

// Initialize all column pins.
for (size_t col = 0; col < MATRIX_SIZE; ++col)
{
    pinMode(COL_PINS[col], OUTPUT);
    digitalWrite(COL_PINS[col], HIGH);
}

clearMatrix();

}

// ============================================
// Main Loop
// ============================================

void loop()
{
// Continuously refresh the LED matrix.
updateMultiplexing();
}