#include <ESP8266WiFi.h>
#include <Cloud4RPi.h>

const String deviceToken = "__YOUR__DEVICE_TOKEN__";
Cloud4RPi c4r(deviceToken);

// WiFi
const char* wifiSSID = "__SSIID__";
const char* wifiPassword = "__PASSWORD__";

int ledPin = 2; // Build-in Led in NodeMCU

WiFiClient wifiClient;
void ensureWiFiConnection();
bool onLEDCommand(bool newValue);

int timerCountDown = 0;
const int  publishPeriod = 15; // in sec

void setup() {
    Serial.begin(9600);
    ensureWiFiConnection();

    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, HIGH);

    c4r.begin(wifiClient);
    c4r.printLogo();
    c4r.ensureConnection();

    c4r.declareBoolVariable("LEDOn", onLEDCommand);
    c4r.declareNumericVariable("Uptime");
    c4r.declareStringVariable("Asterix");

    c4r.publishConfig();
    c4r.loop();

    delay(1000);
}

void loop() {
    ensureWiFiConnection();
    if (c4r.ensureConnection(3)) { // number of attempts
        bool b = c4r.getBoolValue("LEDOn");
        double n = c4r.getNumericValue("Uptime");
        String s = c4r.getStringValue("Asterix");

        Serial.println("BOOL=" + String(b));
        Serial.println("NUM=" + String(n));
        Serial.println("STR=" + s);

        c4r.setVariable("Uptime", millis());
        c4r.setVariable("Asterix",  String(s + "*"));

        c4r.publishData();

        timerCountDown = publishPeriod;
        while(timerCountDown--) {
            c4r.loop();
            
            Serial.print(".");
            delay(1000);
        }
        Serial.println("");
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
    }
}

bool onLEDCommand(bool value) {
    digitalWrite(ledPin, value ? LOW : HIGH);
    int current = digitalRead(ledPin);
    return !(bool)current;
}
