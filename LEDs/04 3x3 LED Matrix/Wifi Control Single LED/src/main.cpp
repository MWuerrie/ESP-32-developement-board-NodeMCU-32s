#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>



// ============================================
// Targeting Single LED with Wifi (Access from PC or phone)
// ============================================


// ============================================
// Configuration
// ============================================

// WiFi credentials
constexpr char WIFI_SSID[] = "your ssid"; 
constexpr char WIFI_PASSWORD[] = "your wifi password";

// MQTT configuration
constexpr char MQTT_SERVER[] = "broker.hivemq.com";
constexpr uint16_t MQTT_PORT = 1883;
constexpr char MQTT_TOPIC[] = "TEST_LED/matrix/led";

// LED matrix GPIO configuration
constexpr uint8_t ROW_PINS[] = {27, 26, 25};
constexpr uint8_t COL_PINS[] = {19, 22, 23};

constexpr size_t MATRIX_SIZE =
sizeof(ROW_PINS) / sizeof(ROW_PINS[0]);

// ============================================
// Global Variables
// ============================================

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Currently active LED.
// -1 means that no LED is active.
int activeRow = -1;
int activeCol = -1;

// ============================================
// WiFi Connection
// ============================================

void connectToWiFi()
{
Serial.println();
Serial.print("Connecting to WiFi: ");
Serial.println(WIFI_SSID);

WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

while (WiFi.status() != WL_CONNECTED)
{
    delay(500);
    Serial.print(".");
}

Serial.println();
Serial.println("WiFi connected.");
Serial.print("IP address: ");
Serial.println(WiFi.localIP());

}

// ============================================
// MQTT Message Callback
// ============================================

// Called automatically when a message is received
// on the subscribed MQTT topic.
void mqttCallback(char* topic, byte* payload, unsigned int length)
{
String message;

for (unsigned int i = 0; i < length; ++i)
{
    message += static_cast<char>(payload[i]);
}

Serial.print("MQTT message received: ");
Serial.println(message);

// Expected message format: "row,column"
// Example: "0,1"
const int separator = message.indexOf(',');

if (separator == -1)
{
    Serial.println("Invalid MQTT message format.");
    return;
}

const int row = message.substring(0, separator).toInt();
const int col = message.substring(separator + 1).toInt();

// Accept only valid matrix coordinates.
if (row >= 0 && row < static_cast<int>(MATRIX_SIZE) &&
    col >= 0 && col < static_cast<int>(MATRIX_SIZE))
{
    activeRow = row;
    activeCol = col;

    Serial.print("Activating LED: ");
    Serial.print(activeRow);
    Serial.print(",");
    Serial.println(activeCol);
}
else
{
    Serial.println("Invalid LED coordinates.");
}

}

// ============================================
// MQTT Connection
// ============================================

void connectToMQTT()
{
while (!mqttClient.connected())
{
Serial.print("Connecting to MQTT broker...");

    // Generate a unique client ID for the ESP32.
    String clientId =
        "ESP32Client-" + String(random(0, 0xFFFF), HEX);

    if (mqttClient.connect(clientId.c_str()))
    {
        Serial.println("connected.");

        // Subscribe to the matrix control topic.
        mqttClient.subscribe(MQTT_TOPIC);

        Serial.print("Subscribed to: ");
        Serial.println(MQTT_TOPIC);
    }
    else
    {
        Serial.print("Connection failed, state=");
        Serial.println(mqttClient.state());
        Serial.println("Retrying in 5 seconds...");

        delay(5000);
    }
}

}

// ============================================
// LED Matrix Initialization
// ============================================

void initializeMatrix()
{
// Initialize row and column GPIOs.
// LOW on rows and HIGH on columns means
// that all LEDs are switched off.
for (size_t i = 0; i < MATRIX_SIZE; ++i)
{
pinMode(ROW_PINS[i], OUTPUT);
digitalWrite(ROW_PINS[i], LOW);

    pinMode(COL_PINS[i], OUTPUT);
    digitalWrite(COL_PINS[i], HIGH);
}

}

// ============================================
// LED Matrix Control
// ============================================

void updateMatrix()
{
// Switch all LEDs off before activating
// the selected LED. This prevents ghosting.
for (size_t i = 0; i < MATRIX_SIZE; ++i)
{
digitalWrite(ROW_PINS[i], LOW);
digitalWrite(COL_PINS[i], HIGH);
}

// Activate the selected LED if the coordinates
// are valid.
if (activeRow >= 0 &&
    activeRow < static_cast<int>(MATRIX_SIZE) &&
    activeCol >= 0 &&
    activeCol < static_cast<int>(MATRIX_SIZE))
{
    // Short blanking interval to allow the
    // GPIO states to stabilize.
    delayMicroseconds(10);

    digitalWrite(ROW_PINS[activeRow], HIGH);
    digitalWrite(COL_PINS[activeCol], LOW);
}

}

// ============================================
// Setup
// ============================================

void setup()
{
Serial.begin(115200);

initializeMatrix();

connectToWiFi();

mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
mqttClient.setCallback(mqttCallback);

}

// ============================================
// Main Loop
// ============================================

void loop()
{
// Reconnect to the MQTT broker if necessary.
if (!mqttClient.connected())
{
connectToMQTT();
}

// Process incoming MQTT messages.
mqttClient.loop();

// Update the LED matrix.
updateMatrix();

}