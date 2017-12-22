#include "Cloud4RPi.h"

Cloud4RPi::Cloud4RPi(const String &_deviceToken, const String &_server, int _port) :
    deviceToken(_deviceToken),
    server(_server),
    port(_port),
    mqttClient(NULL) {
      // Empty
}

void Cloud4RPi::begin(Client& _client) {
  mqttClient = new PubSubClient(_client);
  mqttClient->setServer(server.c_str(), port);
}

Cloud4RPi::~Cloud4RPi() {
  if (mqttClient == NULL) {
    delete mqttClient;
    mqttClient = NULL;
  };
}

bool Cloud4RPi::loop() {
    return mqttClient->loop();
}

bool Cloud4RPi::connected() {
    return mqttClient->connected();
}

bool Cloud4RPi::ensureConnection(int maxReconnectAttempts, int reconnectTimeout) {
    int attempt = 0;
    bool forever = maxReconnectAttempts == RETRY_FOREVER;
    while (!this->connected()) {
        if (!forever && attempt > maxReconnectAttempts) {
            break;
        }
        Serial.print("Attempting to connect to Cloud4PRi broker, (#");
        Serial.print(++attempt);
        Serial.println(")...");
        if (mqttClient->connect(deviceToken.c_str())) {
            Serial.println("Connected!");
            return true;
        } else {
            Serial.print("Connect Failed!, rc=");
            Serial.print(mqttClient->state());
            Serial.print(". Trying again in ");
            Serial.print(reconnectTimeout);
            Serial.println(" second...");
            delay(reconnectTimeout);
        }
    }
    return false;
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
