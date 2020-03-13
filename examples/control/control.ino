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

const int dataSendingInterval = 60000; // milliseconds
const int diagSendingInterval = 90000; // milliseconds
const int pollInterval = 1000; // milliseconds

unsigned long dataTimer = 0;
unsigned long diagTimer = 0;

const int eventCount = 3;
String events[eventCount] = {
    "IDLE",
    "RING",
    "BOOM"
};

Cloud4RPi c4r(deviceToken);
WiFiClient wifiClient;

void ensureWiFiConnection();
String uptimeHumanReadable(unsigned long milliseconds);
bool onLEDCommand(bool value);
double onDesiredTempCommand(double value);

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    ensureWiFiConnection();

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    c4r.begin(wifiClient);
    
    c4r.ensureConnection();
    c4r.printLogo();

    c4r.declareBoolVariable("LED On", onLEDCommand);
    c4r.declareStringVariable("State");
    c4r.declareNumericVariable("DesiredTemp", onDesiredTempCommand);
    c4r.setVariable("DesiredTemp", 22.5f);

    c4r.publishConfig();

    c4r.declareDiagVariable("IP Address");
    c4r.declareDiagVariable("RSSI"); // WiFi signal strength
    c4r.declareDiagVariable("Uptime");

    String v = c4r.getVersion();
    c4r.declareDiagVariable("Client Version");
    c4r.setDiagVariable("Client Version", v);
    Serial.print("Cloud4RPi version: ");
    Serial.println(v);

    c4r.loop();
    delay(1000);
}

void loop() {
    ensureWiFiConnection();

    if (c4r.ensureConnection(3)) { // number of attempts
        unsigned long currentMillis = millis();
        
        if (dataTimer <= 0) {
            Serial.println();
            String newEvent = events[random(eventCount)];
            c4r.setVariable("State", newEvent);

            c4r.publishData();
            dataTimer = dataSendingInterval;

            Serial.println(F("Variables state:"));
            Serial.print(F("  LED = "));
            Serial.println(c4r.getBoolValue("LED On") ? "On" : "Off");
            Serial.print(F("  State = "));
            Serial.println(newEvent);
            Serial.print(F("  Desired Temperature = "));
            Serial.println(c4r.getNumericValue("DesiredTemp"), 1);
        }

        if (diagTimer <= 0) {
            Serial.println();
            c4r.setDiagVariable("RSSI", (String)WiFi.RSSI() + " dBm");
            c4r.setDiagVariable("IP Address", WiFi.localIP().toString());
            c4r.setDiagVariable("Uptime", uptimeHumanReadable(currentMillis));

            c4r.publishDiag();
            diagTimer = diagSendingInterval;
        }

        c4r.loop();
        Serial.print(".");

        diagTimer -= pollInterval;
        dataTimer -= pollInterval;
        
        delay(pollInterval);
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
        Serial.println(WiFi.localIP());
        //WiFi.printDiag(Serial);

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

String uptimeHumanReadable(unsigned long milliseconds) {
    static char uptimeStr[32];
    unsigned long secs = milliseconds / 1000;
    unsigned long mins = secs / 60;
    unsigned int hours = mins / 60;
    unsigned int days = hours / 24;
    secs -= mins * 60;
    mins -= hours * 60;
    hours -= days * 24;
    sprintf(uptimeStr,"%d days %2.2d:%2.2d:%2.2d", (byte)days, (byte)hours, (byte)mins, (byte)secs);
    return String(uptimeStr);
}
