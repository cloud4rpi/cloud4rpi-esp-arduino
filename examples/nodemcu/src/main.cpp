#include <ESP8266WiFi.h>
#include <cloud4rpi.h>

#define POLL_INTERVAL 10000
#define DATA_SENDING_INTERVAL 6000

const char* wifiSSID = "__SSID__";
const char* wifiPassword = "__PASS__";
const char* DEVICE_TOKEN = "__YOUR__DEVICE_TOKEN__";

WiFiClient wifiClient;
Cloud4RPi c4r(DEVICE_TOKEN, &wifiClient);

int timerCountDown = 0;

void printWiFiInfo() {
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    // print your MAC address:
    byte mac[6];
    WiFi.macAddress(mac);
    Serial.print("MAC address: ");
    Serial.print(mac[5], HEX);
    Serial.print(":");
    Serial.print(mac[4], HEX);
    Serial.print(":");
    Serial.print(mac[3], HEX);
    Serial.print(":");
    Serial.print(mac[2], HEX);
    Serial.print(":");
    Serial.print(mac[1], HEX);
    Serial.print(":");
    Serial.println(mac[0], HEX);        
}

void checkWiFiConnection() {
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(wifiSSID, wifiPassword);
        while (WiFi.status() != WL_CONNECTED) {
            Serial.println("Connecting to Wi-Fi...");
            delay(2000);
        }
        Serial.print("Connected! ");
        printWiFiInfo();
    }
}

Cloud4RPiVar variables[] = {
    { name: (char*)"Uptime", type: (char*)"numeric", value: "0" },
    { name: (char*)"Status", type: (char*)"string", value: "Unknown" }
};

void setup() {
    Serial.begin(9600);
    checkWiFiConnection();

    c4r.printLogo();
    c4r.ensureConnection();
    int count = sizeof(variables) / sizeof(Cloud4RPiVar);
    c4r.declareVariables(variables, count);
    c4r.publishConfig();
    c4r.loop();
    delay(1000);
}

void loop() {
    checkWiFiConnection();
    c4r.ensureConnection();
    
    if (timerCountDown <= 0) {
        // collect and prepare data 
        c4r.setVariable("Uptime", String(millis(), DEC));
        c4r.setVariable("Status", "Online");
        c4r.publishData();
        timerCountDown = DATA_SENDING_INTERVAL;
    } 
    c4r.loop();
    delay(POLL_INTERVAL);
    timerCountDown -= POLL_INTERVAL;
}
