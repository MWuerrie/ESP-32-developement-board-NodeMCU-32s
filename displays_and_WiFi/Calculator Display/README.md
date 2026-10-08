# ESP-32 WiFi Calculator Display

A simple calculator with the basic algebraic operators +,-,*,/ . The ESP32 is connected via a shift register to an 4-digit 7-Segment and an additionally OLED display. 

In the python script you enter the two numbers and the operator.

The 4- digit 7-segment display is showing the final result. On the OLED display the calculation is represented as well.


## Setup

* **IDE:** VS Code with extension **PlatformIO IDE**.
* **Framework:** Arduino
* **Hardware:** 
- ESP32 Development Board (NodeMCU ESP32)
- 4-digit 7-segment display (3641AS)
- OLED display
- 74HC595 shift register
- 10 resistors 220 Ohm

## Schematic 
===================================================================
COMPLETE WIRING DIAGRAM: ESP32 + SN74HC595N + 4-DIGIT DISPLAY + OLED + 2 LEDs
===================================================================
System Voltage: 3.3V (Safe operation for ESP32 and shift register)

POWER SUPPLY NOTE:
Use the long lateral power rails (+ and -) of your breadboard.
- ESP32 Pin '3V3' -> Connect to the positive rail (+) of the breadboard
- ESP32 Pin 'GND' -> Connect to the negative rail (-) of the breadboard

-------------------------------------------------------------------
1. CONNECTING THE SHIFT REGISTER (SN74HC595N)
-------------------------------------------------------------------
IC Pin 8 (GND)       -> Negative rail (-) [Ground]
IC Pin 10 (/MR)      -> Positive rail (+)  [Master Reset disabled]
IC Pin 11 (SHCP)     -> ESP32 GPIO 22     [Shift Register Clock]
IC Pin 12 (STCP)     -> ESP32 GPIO 21      [Storage Register Clock / Latch]
IC Pin 13 (/OE)      -> Negative rail (-) [Output Enable permanently active]
IC Pin 14 (DS)       -> ESP32 GPIO 23     [Serial Data Input / Data Line]
IC Pin 16 (VCC)      -> Positive rail (+)  [3.3V Power Supply]

-------------------------------------------------------------------
2. SHIFT REGISTER (OUTPUTS) TO 7-SEGMENT DISPLAY (SEGMENTS)
-------------------------------------------------------------------
* Important: Each segment requires its own 220 Ohm current-limiting resistor!
* Connect each resistor between the IC pin and the corresponding display pin.

IC Pin 15 (Q0) -> [220 Ohm Resistor] -> Segment A
IC Pin 1  (Q1) -> [220 Ohm Resistor] -> Segment B
IC Pin 2  (Q2) -> [220 Ohm Resistor] -> Segment C
IC Pin 3  (Q3) -> [220 Ohm Resistor] -> Segment D
IC Pin 4  (Q4) -> [220 Ohm Resistor] -> Segment E
IC Pin 5  (Q5) -> [220 Ohm Resistor] -> Segment F
IC Pin 6  (Q6) -> [220 Ohm Resistor] -> Segment G
IC Pin 7  (Q7) -> [220 Ohm Resistor] -> Decimal Point (DP)

-------------------------------------------------------------------
3. ESP32 TO 7-SEGMENT DISPLAY (DIGITS)
-------------------------------------------------------------------
* These pins control the 4 individual digits via multiplexing.
* Connect directly (without resistor).

ESP32 GPIO 14 -> Digit 1 (far left digit)
ESP32 GPIO 27 -> Digit 2
ESP32 GPIO 26 -> Digit 3
ESP32 GPIO 25 -> Digit 4 (far right digit)

-------------------------------------------------------------------
4. OLED DISPLAY
-------------------------------------------------------------------
GND        -> Negative rail (-) [Ground]
VCC/VDD    -> Positive rail (+)  [3.3V Power Supply]
SCK        -> GPIO 32 
SDA        -> GPIO 33

-------------------------------------------------------------------
5. ADDITIONAL LEDS ON THE ESP32
-------------------------------------------------------------------
* Current-limiting resistors are strictly required here as well to protect the ESP32.

LED 1:
- ESP32 GPIO 18 -> [220 Ohm Resistor] -> Anode (long leg) of LED 1
- Cathode (short leg) of LED 1   -> Directly to negative rail (-)

LED 2:
- ESP32 GPIO 19 -> [220 Ohm Resistor] -> Anode (long leg) of LED 2
- Cathode (short leg) of LED 2   -> Directly to negative rail (-)

===================================================================

![Schematic](../Hardware/displays.png)

## Media

[▶️ Video](Media/WiFi Calculator display.mp4)



## Code (`src/main.cpp`)

```cpp
Look up in src
```


## Execute

1. Connect the ESP with USB for Power supply

2. Change in the python script (Wifi calculator) the ESP32 IP:

=====================================================
 ESP32 wifi address & Port
=====================================================

ESP32_IP = "192.X.X.X"     # IP address of your ESP32
ESP32_PORT = XXXX          # Port, e.g. 5000

3. Change in the main.cpp your WiFI SSID and password and choose the Port

// =====================================================
// WiFi Configuration
// =====================================================



const char* WIFI_SSID = "Your SSID";    
const char* WIFI_PASSWORD = "Your WiFi password";

// server(xxxx), xxxx = Port
// server is here a variable, we could also name it e.g. "my_server"

WiFiServer server(xxxx);


4. Run the python file

5. Choose first number, operator and second number

6. Result is shown on the displays
