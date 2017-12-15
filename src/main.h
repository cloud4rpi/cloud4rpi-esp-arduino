#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#define CLOUD4RPI_SERVER "mq.cloud4rpi.io"

#define RECONNECT_TIMEOUT 3000
#define DISABLE_RETRY 0
#define RETRY_FOREVER -1
#define JSON_BUFFER_SIZE 500;

typedef struct { 
    char* name;
    char* type;   
    String value;
} Cloud4RPiVar;

class Cloud4RPi {
public:
    Cloud4RPi(const char *deviceToken, Client *client, const char* server = CLOUD4RPI_SERVER);
    /**
    * Checks the MQTT-broker connectivity and reconnects if neseccery.
    * @param reconnectAttempts < 0 => Retry forever
    *                          0  => Disables re-connection, try to connect once
    *                          > 0 => Number of reconnection attempts
    * @param reconnectTimeout - time in ms before trying to connect to the broker again. 
    * @return The resulting connection status
    */    
    bool ensureConnection(int maxReconnectAttempts = RETRY_FOREVER, int reconnectTimeout = RECONNECT_TIMEOUT);
    bool connected();
    bool loop();

    void declareVariables(Cloud4RPiVar variables[], int variableCount);
    String getVariable(const char *varName);
    void setVariable(const char *varName, String value);

    bool publishConfig();
    bool publishData();
    void printLogo();    
private:
    const char *_server;
    const char *_deviceToken;
    PubSubClient *_mqttClient;
    
    int _jsonBufferSize;
    String currentTime;
    
    Cloud4RPiVar *_variables;
    int _variableCount;
    bool publishCore(JsonObject& root, const char *kind);
    JsonObject& createPayloadBuffer(int size);
};
    
