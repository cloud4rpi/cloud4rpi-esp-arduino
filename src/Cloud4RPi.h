#ifndef Cloud4RPi_h
#define Cloud4RPi_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "Cloud4RPiVar.h"

namespace {
    const String& MQTT_SERVER = "mq.cloud4rpi.io";
    const int MQTT_PORT = 1883;

    const String& C4R_VAR_BOOL = "bool";
    const String& C4R_VAR_NUMERIC = "numeric";
    const String& C4R_VAR_STRING = "string";
}

const int C4R_RECONNECT_TIMEOUT = 3000; // milliseconds
const byte C4R_RETRY_FOREVER = -1;

class Cloud4RPi {
friend class C4RMqttCallback;

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
    bool ensureConnection(int maxReconnectAttempts = C4R_RETRY_FOREVER, int reconnectTimeout = C4R_RECONNECT_TIMEOUT);
    bool connected();
    bool loop();

    void declareBoolVariable(const String& varName, C4R_HANDLER_SIGNATURE = NULL);
    void declareNumericVariable(const String& varName);
    void declareStringVariable(const String& varName);

    bool getBoolValue(const String& varName);
    double getNumericValue(const String& varName);
    String getStringValue(const String& varName);

    void setVariable(const String& varName, bool value);
    void setVariable(const String& varName, int value);
    void setVariable(const String& varName, unsigned int value);
    void setVariable(const String& varName, long int value);
    void setVariable(const String& varName, unsigned long value);
    void setVariable(const String& varName, float value);
    void setVariable(const String& varName, double value);
    void setVariable(const String& varName, String value);

    bool publishConfig();
    bool publishData();

    void printLogo();

private:
    String deviceToken;
    String server;
    int port;
    PubSubClient* mqttClient;
    C4RVariableStorage* variables;
    C4RVariableBase* last;
    int jsonBufferSize;

    bool isVariableExists(const String& varName);
    bool publishCore(JsonObject& root, const String& subTopic);
    JsonVariant getVariantValue(const String& name, const String& type);
    void mqttCallback(char* topic, byte* payload, unsigned int length);
    void onCommand(const String& command, bool value);
};
#endif
