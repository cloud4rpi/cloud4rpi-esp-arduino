#include "cloud4rpi.h"

Cloud4RPi::Cloud4RPi(const char *deviceToken, const char* server) {
    _deviceToken = deviceToken;
    _server = server;
}
void Cloud4RPi::printLogo() {
    Serial.println("");
    Serial.println("_________ .__                   .___ _________________________.__ ");
    Serial.println("\\_   ___ \\|  |   ____  __ __  __| _//  |  \\______   \\______   \\__|");
    Serial.println("/    \\  \\/|  |  /  _ \\|  |  \\/ __ |/   |  ||       _/|     ___/  |");
    Serial.println("\\     \\___|  |_(  <_> )  |  / /_/ /    ^   /    |   \\|    |   |  |");
    Serial.println(" \\______  /____/\\____/|____/\\____ \\____   ||____|_  /|____|   |__|");
    Serial.println("        \\/                       \\/    |__|       \\/");
}
