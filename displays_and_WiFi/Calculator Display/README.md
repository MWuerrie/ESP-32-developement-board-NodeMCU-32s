# ESP-32 WiFi Calculator Display
A small calculator project demonstrating **TCP communication between Python and an ESP32**.

The Python program acts as the **TCP client** and performs the calculation with simple algebraic operations (+,-,*,/) . The ESP32 acts as the **TCP server** and receives the numbers, operator and final result.

The result is displayed on a 4-digit 7-segment display and additionally on an OLED display.

This project was developed as a learning project and as a prototype for a later IoT application.

## Features

* Wi-Fi communication between Python and ESP32
* TCP client/server communication
* Addition, subtraction, multiplication and division
* Input validation on the Python side
* 4-digit 7-segment display output
* Newline-delimited application messages
* TCP acknowledgement (`OK`) from ESP32
* ESP32 OLED display support

---

## System Overview

```text
┌──────────────────────┐
│        Python        │
│                      │
│  TCP Client          │
│  User Input          │
│  Calculation         │
└──────────┬───────────┘
           │
           │ TCP / Wi-Fi
           │
           ▼
┌──────────────────────┐
│        ESP32         │
│                      │
│  TCP Server          │
│  Message Processing  │
│  Display Control     │
└───────┬───────┬──────┘
        │       │
        ▼       ▼
   7-Segment   OLED
    Display   Display
```

The calculation itself is performed by **Python**. The ESP32 is responsible for receiving the data and controlling the hardware displays.


## Software

### Python

* Python 3
* Standard library `socket`

### ESP32

* PlatformIO
* Arduino framework
* ESP32 Wi-Fi library
* Adafruit GFX
* Adafruit SSD1306



## Hardware

### ESP32

* NodeMCU-32S / ESP32
* Wi-Fi
* TCP server

### 4-Digit 7-Segment Display

* 3641AS
* Common-anode
* Controlled through an SN74HC595N shift register

### OLED

* 128 × 64 OLED
* I²C
* Address: `0x3C`

### 74HC595 shift register

### 10 resistors 220 Ohm

## Schematic 

COMPLETE WIRING DIAGRAM: ESP32 + SN74HC595N + 4-DIGIT DISPLAY + OLED + 2 LEDs

System Voltage: 3.3V (Safe operation for ESP32 and shift register)

POWER SUPPLY NOTE:
Use the long lateral power rails (+ and -) of your breadboard.
- ESP32 Pin '3V3' -> Connect to the positive rail (+) of the breadboard
- ESP32 Pin 'GND' -> Connect to the negative rail (-) of the breadboard

-------------------------------------------------------------------
1. CONNECTING THE SHIFT REGISTER (SN74HC595N)
-------------------------------------------------------------------

| IC Pin | Signal | Connection | Description |
|---:|---|---|---|
| 8  | GND  | Negative rail (-) | Ground |
| 10 | /MR  | Positive rail (+) | Master Reset disabled |
| 11 | SHCP | ESP32 GPIO 22 | Shift Register Clock |
| 12 | STCP | ESP32 GPIO 21 | Storage Register Clock / Latch |
| 13 | /OE  | Negative rail (-) | Output Enable permanently active |
| 14 | DS   | ESP32 GPIO 23 | Serial Data Input / Data Line |
| 16 | VCC  | Positive rail (+) | 3.3V Power Supply |


-------------------------------------------------------------------
2. SHIFT REGISTER (OUTPUTS) TO 7-SEGMENT DISPLAY (SEGMENTS)
-------------------------------------------------------------------
* Important: Each segment requires its own 220 Ohm current-limiting resistor!
* Connect each resistor between the IC pin and the corresponding display pin.

| IC Pin | Output | Connection | Display Segment |
|---:|---|---|---|
| 15 | Q0 | 220 Ω resistor | Segment A |
| 1 | Q1 | 220 Ω resistor | Segment B |
| 2 | Q2 | 220 Ω resistor | Segment C |
| 3 | Q3 | 220 Ω resistor | Segment D |
| 4 | Q4 | 220 Ω resistor | Segment E |
| 5 | Q5 | 220 Ω resistor | Segment F |
| 6 | Q6 | 220 Ω resistor | Segment G |
| 7 | Q7 | 220 Ω resistor | Decimal Point (DP) |

-------------------------------------------------------------------
3. ESP32 TO 7-SEGMENT DISPLAY (DIGITS)
-------------------------------------------------------------------
* These pins control the 4 individual digits via multiplexing.
* Connect directly (without resistor).

| ESP32 GPIO | Display Connection | Description |
|---:|---|---|
| GPIO 14 | Digit 1 | Far left digit |
| GPIO 27 | Digit 2 | — |
| GPIO 26 | Digit 3 | — |
| GPIO 25 | Digit 4 | Far right digit |


-------------------------------------------------------------------
4. OLED DISPLAY
-------------------------------------------------------------------
| OLED Pin | ESP32 Connection | Description |
|---|---|---|
| GND | Negative rail (-) | Ground |
| VCC / VDD | Positive rail (+) | 3.3 V Power Supply |
| SCK | GPIO 32 | Clock |
| SDA | GPIO 33 | Data |

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

[Pic](Media/WiFi_Calculator_display.png)



## Code (`src/main.cpp`)

```cpp
Look up in src
```

## How It Works

1. Python connects to the ESP32 using TCP.
2. Python sends a request for the first number.
3. The user enters the first number.
4. Python sends the number to the ESP32.
5. Python requests the operator.
6. The user selects `+`, `-`, `*` or `/`.
7. Python sends the operator to the ESP32.
8. Python requests the second number.
9. The user enters the second number.
10. Python performs the calculation.
11. Python sends the final result to the ESP32.
12. The ESP32 displays the result.
13. The ESP32 sends `OK`.
14. Python receives `OK` and closes the connection.

---

## Example

Input:

```text
First number: 656
Operator (+,-,*,/): +
Second number: 878
```

Python calculates:

```text
656 + 878 = 1534
```

The ESP32 receives:

```text
result:1534
```

The 7-segment display shows:

```text
1534
```

The ESP32 then responds:

```text
OK
```

---

## Project Purpose

This project is a small practical exercise for learning:

* Python networking
* TCP/IP communication
* Client-server architecture
* ESP32 programming
* Wi-Fi communication
* Simple application-layer protocols
* Message parsing
* Hardware display control
* Acknowledgement mechanisms




## Execution Notifications

1. Connect the ESP with USB for Power supply

2. Change in the python script (Wifi calculator) the ESP32 IP:


 ESP32 wifi address & Port


ESP32_IP = "192.X.X.X"     # IP address of your ESP32

ESP32_PORT = XXXX          # Port, e.g. 5000

3. Change in the main.cpp your WiFI SSID and password and choose the Port


WiFi Configuration

const char* WIFI_SSID = "Your SSID";  

const char* WIFI_PASSWORD = "Your WiFi password";

server(xxxx), xxxx = Port



WiFiServer server(xxxx);



