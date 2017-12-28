#include <ESP8266WiFi.h>
#include <Cloud4RPi.h>

const String deviceToken = "__YOUR__DEVICE_TOKEN__";

Cloud4RPi c4r(deviceToken);

// WiFi
const char* wifiSSID = "__SSIID__";
const char* wifiPassword = "__PASSWORD__";

WiFiClient wifiClient;
void ensureWiFiConnection();

int timerCountDown = 0;
const int  publishPeriod = 15; // in sec

void setup() {
    Serial.begin(9600);
    ensureWiFiConnection();

    c4r.begin(wifiClient);
    c4r.printLogo();

    //c4r.ensureConnection();
    c4r.declareBoolVariable("On/Off");
    c4r.declareNumericVariable("Uptime");
    c4r.declareStringVariable("Asterix");

    //c4r.publishConfig();
    c4r.loop();
    delay(1000);
}

void loop() {
   ensureWiFiConnection();
  if (1 || c4r.ensureConnection(3)) { // number of attempts

    bool b = c4r.getBoolValue("On/Off");
    double n = c4r.getNumericValue("Uptime");
    String s = c4r.getStringValue("Asterix");

    Serial.println("BOOL=" +  String(b));
    Serial.println("NUM=" +  String(n));
    Serial.println("STR=" +  String(s));

    c4r.setVariable("On/Off",  !b);
    c4r.setVariable("Uptime", millis());
    c4r.setVariable("Asterix",  String(s + "*"));

    timerCountDown = publishPeriod;
    while(timerCountDown--) {
      c4r.loop();
      delay(1000);
    }
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
