#include <Cloud4RPi.h>

const char* DEVICE_TOKEN = "__YOUR__DEVICE_TOKEN__";
Cloud4RPi c4r(DEVICE_TOKEN);

void setup() {
    Serial.begin(9600);
    c4r.printLogo();
}

void loop() {
}