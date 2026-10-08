#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>  


// =====================================================
// WiFi / Connection to client.py
// =====================================================

const char* WIFI_SSID     = "YOUR SSID";
const char* WIFI_PASSWORD = "YOUR WiFi password";

const int LISTEN_PORT = 5000;   // client.py connect with ESP32-IP:5000

WiFiServer server(LISTEN_PORT);
WiFiClient client;
String rxBuffer;
bool serverOnline = false;      // true, while client.py connected


// =====================================================
// 74HC595 GPIOs
// =====================================================

#define DATA_PIN  23
#define CLOCK_PIN 22
#define LATCH_PIN 21


// =====================================================
// 3641AS GPIOs (DIGITS, LOW = activ)
// =====================================================

#define DIGIT1 14
#define DIGIT2 27
#define DIGIT3 26
#define DIGIT4 25

const int DIGIT_PINS[4] = { DIGIT1, DIGIT2, DIGIT3, DIGIT4 };

const unsigned long DIGIT_INTERVAL_MS = 2;   // pro Ziffer
unsigned long lastDigitUpdate = 0;
int currentDigit = 0;

// Dezimalpunkt hinter Ziffer 2 als Trenner: "01.03"
const bool SHOW_DP_SEPARATOR = true;


// =====================================================
// LEDs (LED1 = Spieler 1, LED2 = Spieler 2)
// =====================================================

#define LED1 19
#define LED2 18

const unsigned long LED_ON_MS = 1000;
unsigned long led1OffAt = 0;
unsigned long led2OffAt = 0;


// =====================================================
// OLED
// =====================================================

#define SDA_PIN 33
#define SCL_PIN 32

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS  0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool oledOk = false;
bool oledDirty = true;


// =====================================================
// Spielstand
// =====================================================

int score1 = 0;   // letzter Stand vom Server
int score2 = 0;

int show1 = 0;    // aktuell angezeigter Stand (bleibt beim Gewinner-Screen eingefroren)
int show2 = 0;

bool waitingForPlayers = true;

int winner = 0;                                  // 0 = kein Gewinner-Screen
unsigned long winnerUntil = 0;                   // 0 = Gewinner-Screen aus
const unsigned long WINNER_SHOW_MS = 5000;


// =====================================================
// 3641AS
// =====================================================

void allDigitsOff()
{
    for (int i = 0; i < 4; i++)
    {
        digitalWrite(DIGIT_PINS[i], HIGH);
    }
}

void sendSegments(byte pattern)
{
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, pattern);
    digitalWrite(LATCH_PIN, HIGH);
}

// Q7 Q6 Q5 Q4 Q3 Q2 Q1 Q0
// DP  G  F  E  D  C  B  A
byte digitPattern(int digit)
{
    switch (digit)
    {
        case 0: return 0b00111111;
        case 1: return 0b00000110;
        case 2: return 0b01011011;
        case 3: return 0b01001111;
        case 4: return 0b01100110;
        case 5: return 0b01101101;
        case 6: return 0b01111101;
        case 7: return 0b00000111;
        case 8: return 0b01111111;
        case 9: return 0b01101111;
        default: return 0b00000000;
    }
}

// Nicht blockierend, in jedem loop()-Durchlauf aufrufen.
// Anzeige: Spieler 1 = Ziffer 1+2, Spieler 2 = Ziffer 3+4
void updateSevenSegment()
{
    if (millis() - lastDigitUpdate < DIGIT_INTERVAL_MS)
    {
        return;
    }

    lastDigitUpdate = millis();

    int s1 = constrain(show1, 0, 99);
    int s2 = constrain(show2, 0, 99);

    int digits[4] = { s1 / 10, s1 % 10, s2 / 10, s2 % 10 };

    byte pattern = digitPattern(digits[currentDigit]);

    if (SHOW_DP_SEPARATOR && currentDigit == 1)
    {
        pattern |= 0b10000000;
    }

    allDigitsOff();
    sendSegments(pattern);
    digitalWrite(DIGIT_PINS[currentDigit], LOW);

    currentDigit = (currentDigit + 1) % 4;
}


// =====================================================
// OLED
// =====================================================

void showOledText(const char* text)
{
    if (!oledOk)
    {
        return;
    }

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 24);
    display.println(text);
    display.display();
}

void drawOled()
{
    oledDirty = false;

    if (!oledOk)
    {
        return;
    }

    // Der I2C-Transfer dauert einige ms: keine Ziffer dauerhaft leuchten lassen
    allDigitsOff();

    char buf[8];
    snprintf(buf, sizeof(buf), "%02d:%02d",
             constrain(show1, 0, 99),
             constrain(show2, 0, 99));

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("PONG");

    display.setCursor(86, 0);
    display.print(serverOnline ? " online" : "offline");

    if (waitingForPlayers)
    {
        display.setTextSize(2);
        display.setCursor(0, 14);
        display.print("Warte auf");
        display.setCursor(0, 32);
        display.print("Spieler...");

        display.setTextSize(1);
        display.setCursor(0, 54);
        display.print(WiFi.localIP());   // diese IP gibst du in client.py ein
    }
    else if (winnerUntil != 0)
    {
        display.setTextSize(2);
        display.setCursor(0, 18);
        display.print("Spieler ");
        display.print(winner);
        display.setCursor(0, 38);
        display.print("gewinnt!");

        display.setTextSize(1);
        display.setCursor(49, 56);
        display.print(buf);
    }
    else
    {
        display.setTextSize(4);
        display.setCursor(4, 28);
        display.print(buf);
    }

    display.display();
}


// =====================================================
// LEDs
// =====================================================

void updateLeds()
{
    unsigned long now = millis();

    // Gewinner-Modus: LED des Siegers blinkt, danach zurück zum Normalbetrieb
    if (winnerUntil != 0)
    {
        if ((long)(now - winnerUntil) >= 0)
        {
            winnerUntil = 0;
            winner = 0;

            digitalWrite(LED1, LOW);
            digitalWrite(LED2, LOW);
            led1OffAt = 0;
            led2OffAt = 0;

            show1 = score1;
            show2 = score2;
            oledDirty = true;
        }
        else
        {
            bool on = ((now / 250) % 2) == 0;

            digitalWrite(LED1, (winner == 1 && on) ? HIGH : LOW);
            digitalWrite(LED2, (winner == 2 && on) ? HIGH : LOW);
        }

        return;
    }

    if (led1OffAt != 0 && (long)(now - led1OffAt) >= 0)
    {
        digitalWrite(LED1, LOW);
        led1OffAt = 0;
    }

    if (led2OffAt != 0 && (long)(now - led2OffAt) >= 0)
    {
        digitalWrite(LED2, LOW);
        led2OffAt = 0;
    }
}


// =====================================================
// Nachrichten vom Server
// =====================================================

void handleMessage(const String& line)
{
    Serial.print("Received: ");
    Serial.println(line);

    JsonDocument doc;

    if (deserializeJson(doc, line))
    {
        Serial.println("-> invalid JSON");
        return;
    }

    const char* type = doc["type"] | "";

    // ---------------- Punktestand ----------------

    if (strcmp(type, "score") == 0)
    {
        score1 = doc["score1"] | 0;
        score2 = doc["score2"] | 0;
        int scorer = doc["scorer"] | 0;   // 0 = kein Punkt (Reset/Sync)

        Serial.printf("-> Score %d : %d (scorer: %d)\n", score1, score2, scorer);

        // Während des Gewinner-Screens bleibt die Anzeige eingefroren
        if (winnerUntil == 0)
        {
            show1 = score1;
            show2 = score2;

            if (scorer == 1)
            {
                digitalWrite(LED1, HIGH);
                led1OffAt = millis() + LED_ON_MS;
            }
            else if (scorer == 2)
            {
                digitalWrite(LED2, HIGH);
                led2OffAt = millis() + LED_ON_MS;
            }

            oledDirty = true;
        }
    }

    // ---------------- Spielende ----------------

    else if (strcmp(type, "game_over") == 0)
    {
        score1 = doc["score1"] | 0;
        score2 = doc["score2"] | 0;
        show1 = score1;
        show2 = score2;

        winner = doc["winner"] | 0;
        winnerUntil = millis() + WINNER_SHOW_MS;
        waitingForPlayers = false;

        led1OffAt = 0;
        led2OffAt = 0;

        Serial.printf("-> Game over, winner: %d\n", winner);

        oledDirty = true;
    }

    // ---------------- Status (waiting / active) ----------------

    else if (strcmp(type, "status") == 0)
    {
        const char* status = doc["status"] | "";

        waitingForPlayers = (strcmp(status, "waiting") == 0);

        if (waitingForPlayers)
        {
            score1 = 0;
            score2 = 0;
            show1 = 0;
            show2 = 0;

            winner = 0;
            winnerUntil = 0;

            digitalWrite(LED1, LOW);
            digitalWrite(LED2, LOW);
            led1OffAt = 0;
            led2OffAt = 0;
        }

        Serial.printf("-> Status: %s\n", status);

        oledDirty = true;
    }
}

void readNetwork()
{
    while (client.available())
    {
        char c = (char)client.read();

        if (c == '\n')
        {
            rxBuffer.trim();

            if (rxBuffer.length() > 0)
            {
                handleMessage(rxBuffer);
            }

            rxBuffer = "";
        }
        else if (c != '\r' && rxBuffer.length() < 512)
        {
            rxBuffer += c;
        }
    }
}


// =====================================================
// Verbindung
// =====================================================

void acceptClient()
{
    WiFiClient newClient = server.available();

    if (newClient)
    {
        // Eine neue Verbindung ersetzt eine alte (z. B. nach Neustart von client.py)
        client.stop();
        client = newClient;
        rxBuffer = "";

        Serial.println("client.py connected");
    }
}


// =====================================================
// Setup / Loop
// =====================================================

void setup()
{
    Serial.begin(115200);

    // 74HC595
    pinMode(DATA_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(LATCH_PIN, OUTPUT);

    // Ziffern
    for (int i = 0; i < 4; i++)
    {
        pinMode(DIGIT_PINS[i], OUTPUT);
    }

    allDigitsOff();

    // LEDs
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);

    // OLED
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(400000);

    oledOk = display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);

    if (!oledOk)
    {
        Serial.println("OLED not found - continuing without OLED");
    }

    showOledText("Connecting to WiFi...");

    // WiFi
    WiFi.mode(WIFI_STA);
    int n = WiFi.scanNetworks();
    Serial.printf("%d Netze gefunden:\n", n);

    for (int i = 0; i < n; i++)
    {
        Serial.printf("  '%s' (RSSI %d)\n", WiFi.SSID(i).c_str(), WiFi.RSSI(i));
    }

    WiFi.setAutoReconnect(true);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.localIP());

    server.begin();
    Serial.printf("Listening on port %d\n", LISTEN_PORT);

    oledDirty = true;
}

void loop()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        acceptClient();

        if (client.connected())
        {
            readNetwork();
        }
    }

    bool online = client.connected();

    if (online != serverOnline)
    {
        serverOnline = online;
        oledDirty = true;
    }

    updateLeds();
    updateSevenSegment();

    if (oledDirty)
    {
        drawOled();
    }
}