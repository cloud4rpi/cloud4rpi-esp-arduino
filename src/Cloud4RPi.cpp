#include "Cloud4RPi.h"

const int JSON_BUFFER_SIZE = 500;

Cloud4RPi::Cloud4RPi(const String &_deviceToken, const String &_server, int _port) :
    deviceToken(_deviceToken),
    server(_server),
    port(_port),
    mqttClient(NULL),
    jsonBufferSize(JSON_BUFFER_SIZE),
    variables(new C4RVariableStorage()) {
}

Cloud4RPi::~Cloud4RPi() {
    if (mqttClient != NULL) {
        delete mqttClient;
        mqttClient = NULL;
    };

    if (variables != NULL) {
        delete variables;
        variables = NULL;
    }
}
void Cloud4RPi::begin(Client& _client) {
    mqttClient = new PubSubClient(_client);
    mqttClient->setServer(server.c_str(), port);
}

bool Cloud4RPi::loop() {
    return mqttClient->loop();
}

bool Cloud4RPi::connected() {
    return mqttClient->connected();
}

bool Cloud4RPi::ensureConnection(int maxReconnectAttempts, int reconnectTimeout) {
    int attempt = 0;
    bool forever = maxReconnectAttempts <= C4R_RETRY_FOREVER;
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

void Cloud4RPi::declareBoolVariable(const String& varName) {
    if (!isVariableExists(varName)) {
        variables->declare<bool>(varName, C4R_VAR_BOOL);
      }
}

void Cloud4RPi::declareNumericVariable(const String& varName) {
      if (!isVariableExists(varName)) {
        variables->declare<double>(varName, C4R_VAR_NUMERIC);
      }
}

void Cloud4RPi::declareStringVariable(const String& varName) {
    if (!isVariableExists(varName)) {
        variables->declare<char*>(varName, C4R_VAR_STRING);
    }
}
bool Cloud4RPi::isVariableExists(const String& varName) {
    bool exists = variables->exists(varName);
    if (exists) {
        Serial.print("WARN! Duplicate '");
        Serial.print(varName);
        Serial.println("' variable declaration");
    }
    return exists;
}
bool Cloud4RPi::getBoolValue(const String& varName) {
      return variables->getValue<bool>(varName);
}
double Cloud4RPi::getNumericValue(const String& varName) {
      return variables->getValue<double>(varName);
}
String Cloud4RPi::getStringValue(const String& varName) {
      return String(variables->getValue<char*>(varName));
}

void Cloud4RPi::setVariable(const String& varName, bool value) {
  variables->setValue(varName, value);
}

void Cloud4RPi::setVariable(const String& varName, int value) {
  variables->setValue(varName, (double)value);
}

void Cloud4RPi::setVariable(const String& varName, unsigned int value) {
    variables->setValue(varName, (double)value);
}

void Cloud4RPi::setVariable(const String& varName, long value) {
      variables->setValue(varName, (double)value);
}

void Cloud4RPi::setVariable(const String& varName, unsigned long value) {
      variables->setValue(varName, (double)value);
}

void Cloud4RPi::setVariable(const String& varName, float value) {
      variables->setValue(varName, (double)value);
}

void Cloud4RPi::setVariable(const String& varName, double value) {
      variables->setValue(varName, value);
}
void Cloud4RPi::setVariable(const String& varName, String value) {
      variables->setValue(varName, value);
}

bool Cloud4RPi::publishConfig() {
    DynamicJsonBuffer json(jsonBufferSize);
    JsonObject& root = json.createObject();
    JsonArray &payload = root.createNestedArray("payload");

    for(int i = 0; i < variables->size(); i++) {
        JsonObject& item = payload.createNestedObject();
        C4RVariableInfo info = variables->getVariableInfo(i);
        Serial.println(info.name + " " + info.type);
        item["name"] = info.name;
        item["type"] = info.type;
    }
    return this->publishCore(root, "/config");
}

bool Cloud4RPi::publishData() {
}

bool Cloud4RPi::publishCore(JsonObject& root, const String& subTopic) {
    if(!this->connected()) {
        return false;
    }
    String topic = "devices/" + deviceToken + subTopic;
    Serial.print(topic);
    // TODO impl
    // if(currentTime.length() > 0) root["ts"] = currentTime.c_str();
    int length = root.measureLength() + 1;
    char buffer[length];
    root.printTo(buffer, length);
    bool result = mqttClient->publish(topic.c_str(), buffer);

    Serial.print(result ? "[OK ps=" : "[FAIL! ps=");
    //Serial.print(5 + 2 + strlen(topic.c_str()) + strlen(buffer));  //FIXME: Packages longer then 128 (MQTT_MAX_PACKET_SIZE) are failing!
    Serial.print(("] " + topic + " <--- ").c_str());
    Serial.println(buffer);
    return result;
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
