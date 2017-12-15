#include "cloud4rpi.h"

Cloud4RPi::Cloud4RPi(const char *deviceToken, Client *client, const char* server) {
    _deviceToken = deviceToken;
    _server = server;
    _mqttClient = new PubSubClient(*client);
    _mqttClient->setServer(_server, 1883);
    _jsonBufferSize = JSON_BUFFER_SIZE;
}

void Cloud4RPi::declareVariables(Cloud4RPiVar variablesArr[], int variableCount) {
    _variables = variablesArr;
    _variableCount = variableCount;
}

String Cloud4RPi::getVariable(const char *varName) {
    for(int n = 0; n < _variableCount; n++) {
        if(strcmp(_variables[n].name, varName) == 0) {
            return _variables[n].value;
        }
    }
    return "";
}

void Cloud4RPi::setVariable(const char *varName, String value) {
    for(int n = 0; n < _variableCount; n++) {
        if(strcmp(_variables[n].name, varName) == 0) {
            _variables[n].value = value;
        }
    }
}

bool Cloud4RPi::publishCore(JsonObject& root, const char *kind) {
    if(!this->connected()) {
        return false;
    }
    String topic = "devices/" + String(_deviceToken) + kind;
    Serial.print(topic);
    if(currentTime.length() > 0)
        root["ts"] = currentTime.c_str();
    int length = root.measureLength() + 1;
    char buffer[length];
    root.printTo(buffer, length);
    bool result = _mqttClient->publish(topic.c_str(), buffer);

    Serial.print(result ? "[OK ps=" : "[FAIL! ps=");
    Serial.print(5 + 2 + strlen(topic.c_str()) + strlen(buffer));  //FIXME: Packages longer then 128 (MQTT_MAX_PACKET_SIZE) are failing!
    Serial.print(("] " + topic + " <--- ").c_str());
    Serial.println(buffer);
    return result;
}

JsonVariant getJsonVariant(const char *type, const char *value) {
    if(type == "bool") return JsonVariant(strcmp(value, "true") == 0);
    if(type == "numeric") return JsonVariant(atof(value));
    return value;
}

bool Cloud4RPi::publishConfig() {
    DynamicJsonBuffer json(_jsonBufferSize);
    JsonObject& root = json.createObject();
    JsonArray &payload = root.createNestedArray("payload");
    for(int n = 0; n < _variableCount; n++) {
        JsonObject& myVar = payload.createNestedObject();
        myVar["name"] = _variables[n].name;
        myVar["type"] = _variables[n].type;
    }
    return this->publishCore(root, "/config");    
}

bool Cloud4RPi::publishData() {
    DynamicJsonBuffer json(_jsonBufferSize);
    JsonObject& root = json.createObject();
    JsonObject& payload = root.createNestedObject("payload");
    
    for(int n = 0; n < _variableCount; n++) {
        const char *type = _variables[n].type;
        const char *value = _variables[n].value.c_str();
        payload[_variables[n].name] = getJsonVariant(type, value);
    }
    return this->publishCore(root, "/data");
}

bool Cloud4RPi::loop() {
    return _mqttClient->loop();
}

bool Cloud4RPi::connected() {
    return _mqttClient->connected();
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
        if (_mqttClient->connect(_deviceToken)) {
            Serial.println("Connected!");
            return true;
        } else {
            Serial.print("Connect Failed!, rc="); 
            Serial.print(_mqttClient->state());
            Serial.println(". Trying again in 5 second...");
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
