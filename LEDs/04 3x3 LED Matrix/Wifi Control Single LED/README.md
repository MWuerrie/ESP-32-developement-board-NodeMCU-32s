# ESP-32 Online Single Target LED Controller in a 3x3 LED matrix via WiFi

Here we connected the ESP32 with WiFi. Over a HTML, we create a matrix controller, to choose which LED is turned on in the matrix.


## Setup

* **IDE:** VS Code with extension **PlatformIO IDE**.
* **Framework:** Arduino
* **Hardware:** ESP32 Development Board (NodeMCU ESP32) + 9 LEDs in a 3x3 matrix (Common anode) + 3 resistors (220 Ohm)

## Schematic 
GPIO Pins 27, 26 and 25 are connected to Rows 1,2 and 3 of the LED matrix respectively. Columns 1, 2 and 3 of the matrix are connected via a 220 Ohm resistor on ESP32 to the GPIO Pins 31, 36 and 37 respectively.


![Schematic](../Hardware/3x3 LED Matrix.png)

## Media
[▶️ Video](Media/3x3 LED matrix controller.mp4)


## Code (`src/main.cpp`)

```cpp
Look up in src
```



## Target the LED (Execute)

1. In the source code under "configuration" you set your WiFi SSID and your WiFIi password:

// ============================================
// Configuration
// ============================================

// WiFi credentials
constexpr char WIFI_SSID[] = "your ssid"; 
constexpr char WIFI_PASSWORD[] = "your wifi password";

2. Start the HTML 

[🌐 Open web application](HTML/3x3 LED Matrix.html)

3. Now you can use the online matrix controller, to choose the LED. 