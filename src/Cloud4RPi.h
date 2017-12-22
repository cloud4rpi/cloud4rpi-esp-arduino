#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

namespace {
    const String& MQTT_SERVER = "mq.cloud4rpi.io";
    const int MQTT_PORT = 1883;

    const int RECONNECT_TIMEOUT = 3000;
    const byte RETRY_FOREVER = -1;
    const int JSON_BUFFER_SIZE = 500;
}

class Cloud4RPi {
public:
    Cloud4RPi(const String &deviceToken, const String &server = MQTT_SERVER, int port = MQTT_PORT);
    ~Cloud4RPi();

    void begin(Client& client);
    /*
    * Checks the MQTT-broker connectivity and reconnects if neseccery.
    * @param reconnectAttempts < 0 => Retry forever
    *                          0  => Try to connect once
    *                          > 0 => Number of reconnection attempts
    * @param reconnectTimeout - time in ms before trying to connect to the broker again.
    * @return The resulting connection status
    */
    bool ensureConnection(int maxReconnectAttempts = RETRY_FOREVER, int reconnectTimeout = RECONNECT_TIMEOUT);
    bool connected();
    bool loop();

    void printLogo();
private:
    String deviceToken;
    String server;
    int port;
    PubSubClient *mqttClient;
};
