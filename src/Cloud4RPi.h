#ifndef _CLOUD4RPI_H
#define _CLOUD4RPI_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "Cloud4RPiVar.h"

#ifndef CLOUD4RPI_PRINTER
#define CLOUD4RPI_PRINTER Serial
#endif

#ifndef CLOUD4RPI_DEBUG
#define CLOUD4RPI_DEBUG 0
#endif

#if CLOUD4RPI_DEBUG > 0
    #define CLOUD4RPI_PRINT(...) { CLOUD4RPI_PRINTER.print(__VA_ARGS__); }
    #define CLOUD4RPI_PRINTLN(...) { CLOUD4RPI_PRINTER.println(__VA_ARGS__); }
#else
    #define CLOUD4RPI_PRINT(...) {}
    #define CLOUD4RPI_PRINTLN(...) {}
#endif


namespace {
    const String& MQTT_SERVER = "mq.cloud4rpi.io";
    const int MQTT_PORT = 1883;
    const String& VERSION = "1.1.0";
    const String& LANG = "ard";

    const String& VAR_TYPE_BOOL = "bool";
    const String& VAR_TYPE_NUMERIC = "numeric";
    const String& VAR_TYPE_STRING = "string";

    const int JSON_BUFFER_SIZE = 1024;
}

const int C4R_RECONNECT_TIMEOUT = 1000; // milliseconds
const char C4R_RETRY_FOREVER = -1;

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

    void declareBoolVariable(const String& varName, C4R_BOOL_HANDLER_SIGNATURE = NULL);
    void declareNumericVariable(const String& varName, C4R_NUMERIC_HANDLER_SIGNATURE = NULL);
    void declareStringVariable(const String& varName);
    void declareDiagVariable(const String& varName);

    bool getBoolValue(const String& varName);
    double getNumericValue(const String& varName);
    String getStringValue(const String& varName);
    String getDiagValue(const String& varName);

    void setVariable(const String& varName, bool value);
    void setVariable(const String& varName, int value);
    void setVariable(const String& varName, unsigned int value);
    void setVariable(const String& varName, long int value);
    void setVariable(const String& varName, unsigned long value);
    void setVariable(const String& varName, float value);
    void setVariable(const String& varName, double value);
    void setVariable(const String& varName, const char* value);
    void setVariable(const String& varName, const String& value);    
    
    void setDiagVariable(const String& varName, const char* value);
    void setDiagVariable(const String& varName, const String& value);
    
    bool publishConfig();
    bool publishData();
    bool publishDiag();

    void printLogo();
    String getVersion();
private:
    String deviceToken;
    String server;
    int port;
    PubSubClient* mqttClient;
    C4RVariableStorage* variables;
    C4RVariableStorage* diagnostics;
    int jsonBufferSize;

    bool isVariableExists(const String& varName, bool isDiag = false);
    bool publishCore(DynamicJsonDocument doc, const String& subTopic);
    void mqttCallback(char* topic, byte* payload, unsigned int length);
    void onCommand(const String& command, JsonVariant value);
};
#endif // _CLOUD4RPI_H
