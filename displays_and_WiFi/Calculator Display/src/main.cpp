#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// =====================================================
// WiFi Configuration
// =====================================================



const char* WIFI_SSID = "Your SSID";    
const char* WIFI_PASSWORD = "Your WiFi password";

// server(xxxx), xxxx = Port
// server is here a variable, we could also name it e.g. "my_server"
WiFiServer server(5000); 




// =====================================================
// 74HC595
// =====================================================

#define DATA_PIN  23
#define CLOCK_PIN 22
#define LATCH_PIN 21


// =====================================================
// 3641AS display digits
// =====================================================

#define DIGIT1 14
#define DIGIT2 27
#define DIGIT3 26
#define DIGIT4 25


// =====================================================
// LEDs
// =====================================================

#define LED1 19
#define LED2 18


// =====================================================
// OLED
// =====================================================

#define SDA_PIN 33
#define SCL_PIN 32

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    -1
);


// =====================================================
// Display timing
// =====================================================

unsigned long lastDisplayUpdate = 0;

const unsigned long displayInterval = 2;

int currentDigit = 0;


// =====================================================
// Calculator result
// =====================================================

int result = 0;

int firstNumber=0;
int secondNumber =0;

bool waitingForFirstNumber = false;
bool waitingForSecondNumber= false;

String operatorSymbol = "";

// =====================================================
// Turn off all digits
// =====================================================

void allDigitsOff()
{
    digitalWrite(DIGIT1, HIGH);
    digitalWrite(DIGIT2, HIGH);
    digitalWrite(DIGIT3, HIGH);
    digitalWrite(DIGIT4, HIGH);
}


// =====================================================
// Send segment pattern to 74HC595
// =====================================================

void sendSegments(byte pattern)
{
    digitalWrite(LATCH_PIN, LOW);

    shiftOut(
        DATA_PIN,
        CLOCK_PIN,
        MSBFIRST,
        pattern
    );

    digitalWrite(LATCH_PIN, HIGH);
}


// =====================================================
// Segment patterns
//
// Q7 Q6 Q5 Q4 Q3 Q2 Q1 Q0
// DP  G  F  E  D  C  B  A
// =====================================================

byte digitPattern(int digit)
{
    switch (digit)
    {
        case 0:
            return 0b00111111;

        case 1:
            return 0b00000110;

        case 2:
            return 0b01011011;

        case 3:
            return 0b01001111;

        case 4:
            return 0b01100110;

        case 5:
            return 0b01101101;

        case 6:
            return 0b01111101;

        case 7:
            return 0b00000111;

        case 8:
            return 0b01111111;

        case 9:
            return 0b01101111;

        default:
            return 0b00000000;
    }
}


// =====================================================
// Show text on OLED
// =====================================================

void showOLED(String text)
{
    display.clearDisplay();

    display.setTextColor(SSD1306_WHITE);

    display.setTextSize(1);

    display.setCursor(0, 20);

    display.println(text);

    display.display();
}


// =====================================================
// Display result on 3641AS
// =====================================================

void updateResultDisplay()
{
    int thousands = result / 1000;
    int hundreds = (result / 100) % 10;
    int tens = (result / 10) % 10;
    int ones = result % 10;


    // ---------------------------------------------
    // Digit 1
    // ---------------------------------------------

    if (currentDigit == 0)
    {
        sendSegments(
            digitPattern(thousands)
        );

        digitalWrite(DIGIT1, LOW);
    }


    // ---------------------------------------------
    // Digit 2
    // ---------------------------------------------

    else if (currentDigit == 1)
    {
        sendSegments(
            digitPattern(hundreds)
        );

        digitalWrite(DIGIT2, LOW);
    }


    // ---------------------------------------------
    // Digit 3
    // ---------------------------------------------

    else if (currentDigit == 2)
    {
        sendSegments(
            digitPattern(tens)
        );

        digitalWrite(DIGIT3, LOW);
    }


    // ---------------------------------------------
    // Digit 4
    // ---------------------------------------------

    else if (currentDigit == 3)
    {
        sendSegments(
            digitPattern(ones)
        );

        digitalWrite(DIGIT4, LOW);
    }


    currentDigit++;

    if (currentDigit >= 4)
    {
        currentDigit = 0;
    }
}


// =====================================================
// Setup
// =====================================================

void setup()
{
    Serial.begin(115200);


    // =================================================
    // 74HC595
    // =================================================

    pinMode(DATA_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(LATCH_PIN, OUTPUT);


    // =================================================
    // Display digits
    // =================================================

    pinMode(DIGIT1, OUTPUT);
    pinMode(DIGIT2, OUTPUT);
    pinMode(DIGIT3, OUTPUT);
    pinMode(DIGIT4, OUTPUT);

    allDigitsOff();


    // =================================================
    // LEDs
    // =================================================

    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);

    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);


    // =================================================
    // OLED
    // =================================================

    Wire.begin(
        SDA_PIN,
        SCL_PIN
    );


    if (!display.begin(
            SSD1306_SWITCHCAPVCC,
            OLED_ADDRESS))
    {
        while (true)
        {
        }
    }


    showOLED("Calculator");


    // =================================================
    // WiFi
    // =================================================

    WiFi.begin(
        WIFI_SSID,
        WIFI_PASSWORD
    );


    Serial.print(
        "Connecting to WiFi"
    );


    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);

        Serial.print(".");
    }


    Serial.println();

    Serial.println(
        "WiFi connected!"
    );


    Serial.print(
        "ESP32 IP address: "
    );

    Serial.println(
        WiFi.localIP()
    );


    // =================================================
    // TCP server
    // =================================================

    server.begin();


    Serial.println(

        "TCP server started on port 5000"
    );
}








// =====================================================
// Main loop
// =====================================================

void loop()
{
    unsigned long currentMillis = millis();


    // =================================================
    // Check for Python connection
    // =================================================

    WiFiClient client = server.available();


    if (client)
    {
        Serial.println(
            "Python client connected!"
        );


        // =============================================
        // Keep connection open
        // =============================================

        while (client.connected())
        {
            if (client.available())
            {
                String message =
                    client.readStringUntil('\n');

                message.trim();


                Serial.print(
                    "Received: "
                );

                Serial.println(
                    message
                );


                // =====================================
                // First number
                // =====================================

                if (
                    message ==
                    "first number"
                )
                {
                    showOLED(
                        "enter first number"
                    );

                    waitingForFirstNumber = true;
                }

                else if (
                    waitingForFirstNumber
                )
                {
                    firstNumber =
                        message.toInt();

                    waitingForFirstNumber = false;

                    Serial.print(
                        "First number received: "
                    );

                    Serial.println(
                        firstNumber
                    );
                }


                // =====================================
                // Operator question
                // =====================================

                else if (
                    message ==
                    "operator? (+,-,*,/)"
                )
                {
                    showOLED(
                        "choose operator (+,-,*,/)"
                    );
                }


                // =====================================
                // Operator
                // =====================================

                else if (
                    message == "+" ||
                    message == "-" ||
                    message == "*" ||
                    message == "/"
                )
                {
                    operatorSymbol = message;

                    Serial.print(
                        "Operator received: "
                    );

                    Serial.println(
                        operatorSymbol
                    );
                }


                // =====================================
                // Second number
                // =====================================

                else if (
                    message ==
                    "second number"
                )
                {
                    showOLED(
                        "enter second number"
                    );

                    waitingForSecondNumber = true;
                }

                else if (
                    waitingForSecondNumber
                )
                {
                    secondNumber =
                        message.toInt();

                    waitingForSecondNumber = false;

                    Serial.print(
                        "Second number received: "
                    );

                    Serial.println(
                        secondNumber
                    );
                }


                // =====================================
                // Result
                // =====================================

                else if (
                    message.startsWith(
                        "result:"
                    )
                )
                {
                    String resultText =
                        message.substring(7);


                    result =
                        resultText.toInt();


                    Serial.print(
                        "Result received: "
                    );

                    Serial.println(
                        result
                    );

                    client.println("OK");
                    // =================================
                    // Create calculation for OLED
                    // =================================

                    String calculation =
                        String(firstNumber) +
                        " " +
                        operatorSymbol +
                        " " +
                        String(secondNumber) +
                        " = " +
                        String(result);


                    showOLED(
                        calculation
                    );
                }


                // =====================================
                // Error
                // =====================================

                else if (
                    message ==
                    "error"
                )
                {
                    showOLED(
                        "ERROR"
                    );
                }
            }


            // =========================================
            // Update 3641AS
            // =========================================

            if (
                millis() -
                lastDisplayUpdate
                >= displayInterval
            )
            {
                lastDisplayUpdate =
                    millis();

                allDigitsOff();

                updateResultDisplay();
            }


            // =========================================
            // Give ESP32 some time
            // =========================================

            delay(1);
        }


        // =============================================
        // Python closed connection
        // =============================================

        client.stop();


        Serial.println(
            "Python client disconnected."
        );
    }


    // =================================================
    // 3641AS when no client is connected
    // =================================================

    if (
        currentMillis -
        lastDisplayUpdate
        >= displayInterval
    )
    {
        lastDisplayUpdate =
            currentMillis;

        allDigitsOff();

        updateResultDisplay();
    }
}





























