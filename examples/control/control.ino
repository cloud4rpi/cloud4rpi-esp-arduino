#ifdef ESP8266
    #include <ESP8266WiFi.h>
#else
    #include <WiFi.h>
#endif
#include <Cloud4RPi.h>

const String deviceToken = "__YOUR_DEVICE_TOKEN__";
char wifiSSID[] = "__SSID__";
char wifiPassword[] = "__PASSWORD__";

#define SERIAL_BAUD_RATE 9600 // bits per second

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

// Decrease this value for testing purposes.
const int dataSendingInterval = 300000; // milliseconds
const int diagSendingInterval = 60000; // milliseconds
unsigned long lastDataSent = 0;
unsigned long lastDiagSent = 0;

const int eventCount = 3;
String events[eventCount] = {
    "IDLE",
    "RING",
    "BOOM!"
};

Cloud4RPi c4r(deviceToken);
WiFiClient wifiClient;

void ensureWiFiConnection();
bool onLEDCommand(bool value);
double onDesiredTempCommand(double value);

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    ensureWiFiConnection();

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    c4r.begin(wifiClient);
    c4r.printLogo();
    c4r.ensureConnection();

    c4r.declareBoolVariable("LED On", onLEDCommand);
    c4r.declareNumericVariable("Uptime");
    c4r.declareStringVariable("State");
    c4r.declareNumericVariable("DesiredTemp", onDesiredTempCommand);
    c4r.setVariable("DesiredTemp", 22.5f);

    c4r.publishConfig();

    c4r.declareDiagVariable("IP Address");
    c4r.declareDiagVariable("RSSI"); // WiFi signal strength

    c4r.loop();
    delay(1000);
}

void loop() {
    ensureWiFiConnection();

    if (c4r.ensureConnection(3)) { // number of attempts
        unsigned long currentMillis = millis();

        if (currentMillis - lastDataSent >= dataSendingInterval) {
            Serial.println();
            c4r.setVariable("Uptime", currentMillis / 1000);
            String newEvent = events[random(eventCount)];
            c4r.setVariable("State", newEvent);

            c4r.publishData();
            lastDataSent = currentMillis;

            Serial.println("Variables state:");
            Serial.print("  LED = ");
            Serial.println(c4r.getBoolValue("LED On") ? "On" : "Off");
            Serial.print("  Uptime = ");
            Serial.println(c4r.getNumericValue("Uptime"), 0);
            Serial.print("  State = ");
            Serial.println(newEvent);
            Serial.print("  Desired Temperature = ");
            Serial.println(c4r.getNumericValue("DesiredTemp"), 1);
        }

        if (currentMillis - lastDiagSent >= diagSendingInterval) {
            Serial.println();
            c4r.setDiagVariable("RSSI", (String)WiFi.RSSI() + " dBm");
            c4r.setDiagVariable("IP Address", WiFi.localIP().toString());

            c4r.publishDiag();
            lastDiagSent = currentMillis;
        }

        c4r.loop();
        Serial.print(".");
        delay(1000);
    }
}

void ensureWiFiConnection() {
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(wifiSSID, wifiPassword);
        while (WiFi.status() != WL_CONNECTED) {
            Serial.println("Connecting to Wi-Fi...");
            delay(2000);
        }
        Serial.print("Connected! ");
        Serial.print("IP: ");
        Serial.print(WiFi.localIP());
        WiFi.printDiag(Serial);

        long rssi = WiFi.RSSI();  // Received signal strength
        Serial.print("RSSI:");
        Serial.println(rssi);
    }
}

bool onLEDCommand(bool value) {
    Serial.print("LED state set to ");
    Serial.println(value);
    digitalWrite(LED_BUILTIN, value ? HIGH : LOW);
    return digitalRead(LED_BUILTIN);
}

double onDesiredTempCommand(double value) {
    Serial.print("Desired temperature set to ");
    Serial.println(value, 1);
    // Control the heater
    return value;
}
