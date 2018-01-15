#include <ESP8266WiFi.h>
#include <Cloud4RPi.h>

const String deviceToken = "__YOUR__DEVICE_TOKEN__";
Cloud4RPi c4r(deviceToken);

// WiFi
const char* wifiSSID = "__SSIID__";
const char* wifiPassword = "__PASSWORD__";

int ledPin = D4;
int timerCountDown = 0;
const int  publishPeriod = 15; // in sec

const int featureCount = 6;
String features[featureCount] = {
    "Real time messaging using the MQTT protocol",
    "Monitor device connection status",
    "View device diagnostic data",
    "Real time dashboard",
    "Store and view data history",
    "Alerts"
};

WiFiClient wifiClient;
void ensureWiFiConnection();
bool onLEDCommand(bool newValue);

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
    c4r.declareStringVariable("Feature");

    c4r.publishConfig();
    c4r.loop();

    delay(1000);
}

void loop() {
    ensureWiFiConnection();
    if (c4r.ensureConnection(3)) { // number of attempts
        c4r.setVariable("Uptime", millis());
        c4r.setVariable("Feature", features[random(featureCount)]);

        c4r.publishData();

        Serial.println("Variables state: ");
        Serial.print("LEDOn = ");
        Serial.println(c4r.getBoolValue("LEDOn"));
        Serial.print("Uptime = ");
        Serial.println(c4r.getNumericValue("Uptime"));
        Serial.print("Feature = ");
        Serial.println(c4r.getStringValue("Feature"));

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
    return !digitalRead(ledPin);
}
