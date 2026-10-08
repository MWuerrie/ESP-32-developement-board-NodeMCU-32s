# ESP-32 Single LED flashing sequence via Multiplexing

Here we use Multiplexing to target all LEDs in the matrix in a sequence.


## Setup

* **IDE:** VS Code with extension **PlatformIO IDE**.
* **Framework:** Arduino
* **Hardware:** ESP32 Development Board (NodeMCU ESP32) + 9 LEDs in a 3x3 matrix (Common anode) + 3 resistors (220 Ohm)

## Schematic 
GPIO Pins 27, 26 and 25 are connected to Rows 1,2 and 3 of the LED matrix respectively. Columns 1, 2 and 3 of the matrix are connected via a 220 Ohm resistor on ESP32 to the GPIO Pins 31, 36 and 37 respectively.


![Schematic](../Hardware/3x3 LED Matrix.png)
## Code (`src/main.cpp`)

```cpp
Look up in src
```
