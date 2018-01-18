#include "Cloud4RPi.h"

class C4RMqttCallback {
public:
    C4RMqttCallback(Cloud4RPi& _client) :
        client(_client) {
        }
    void operator()(char* topic, byte* payload, unsigned int length) {
        client.mqttCallback(topic, payload, length);
    }
private:
    Cloud4RPi &client;
};

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
    mqttClient->setCallback(C4RMqttCallback(*this));
}

bool Cloud4RPi::loop() {
    if (mqttClient == NULL ) {
        return false;
    }
    return mqttClient->loop();
}

bool Cloud4RPi::connected() {
    if (mqttClient == NULL ) {
        return false;
    }
    return mqttClient->connected();
}

bool Cloud4RPi::ensureConnection(int maxReconnectAttempts, int reconnectTimeout) {
    if (mqttClient == NULL ) {
        return false;
    }
    int attempt = 0;
    bool forever = maxReconnectAttempts <= C4R_RETRY_FOREVER;
    while (!this->connected()) {
        if (!forever && attempt > maxReconnectAttempts) {
            return false;
        }
        CLOUD4RPI_PRINT("Attempting to connect to Cloud4PRi broker, (#");
        CLOUD4RPI_PRINT(++attempt);
        CLOUD4RPI_PRINTLN(")...");
        if (mqttClient->connect(deviceToken.c_str())) {
            CLOUD4RPI_PRINTLN("Subscribing to the 'commands/' topic...");
            String command = "devices/" + deviceToken + "/commands";
            mqttClient->subscribe(command.c_str()); //subscribe to commands
            CLOUD4RPI_PRINTLN("Connected!");
            return true;
        } else {
            CLOUD4RPI_PRINT("Connect Failed!, rc=");
            CLOUD4RPI_PRINT(mqttClient->state());
            CLOUD4RPI_PRINT(". Trying again in ");
            CLOUD4RPI_PRINT(reconnectTimeout);
            CLOUD4RPI_PRINTLN(" second...");
            delay(reconnectTimeout);
        }
    }
    return true;
}

void Cloud4RPi::declareBoolVariable(const String& varName, C4R_HANDLER_SIGNATURE) {
    if (!isVariableExists(varName)) {
        variables->declare<bool>(varName, VAR_TYPE_BOOL, cmdHandler);
     }
}

void Cloud4RPi::declareNumericVariable(const String& varName) {
      if (!isVariableExists(varName)) {
        variables->declare<double>(varName, VAR_TYPE_NUMERIC);
      }
}

void Cloud4RPi::declareStringVariable(const String& varName) {
    if (!isVariableExists(varName)) {
        variables->declare<char*>(varName, VAR_TYPE_STRING);
    }
}
bool Cloud4RPi::isVariableExists(const String& varName) {
    bool exists = variables->exists(varName);
    if (exists) {
        CLOUD4RPI_PRINT("WARN! Duplicate '");
        CLOUD4RPI_PRINT(varName);
        CLOUD4RPI_PRINTLN("' variable declaration");
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
        item["name"] = info.name;
        item["type"] = info.type;
    }
    return this->publishCore(root, "/config");
}

JsonVariant Cloud4RPi::getVariantValue(const String& name, const String& type) {
    if(type == VAR_TYPE_BOOL) {
      return this->getBoolValue(name);
    }
    if(type == VAR_TYPE_NUMERIC) {
      return this->getNumericValue(name);
    }
    if(type == VAR_TYPE_STRING) {
      return variables->getValue<char*>(name);
      //FIXME - do not use this->getStringValue(name).c_str() ;
    }
    return NULL;
}

bool Cloud4RPi::publishData() {
    DynamicJsonBuffer json(jsonBufferSize);
    JsonObject& root = json.createObject();
    JsonObject &payload = root.createNestedObject("payload");

    for(int i = 0; i < variables->size(); i++) {
        C4RVariableInfo info = variables->getVariableInfo(i);
        payload[info.name] = this->getVariantValue(info.name, info.type);
    }
    return this->publishCore(root, "/data");
}

bool Cloud4RPi::publishCore(JsonObject& root, const String& subTopic) {
    if(!this->connected()) {
        return false;
    }
    String topic = "devices/" + deviceToken + subTopic;
    // TODO impl
    // if(currentTime.length() > 0) root["ts"] = currentTime.c_str();
    int length = root.measureLength() + 1;
    char buffer[length];
    root.printTo(buffer, length);
    CLOUD4RPI_PRINT((subTopic + " <-- ").c_str());
    CLOUD4RPI_PRINTLN(buffer);
    bool result = mqttClient->publish(topic.c_str(), buffer);
    if (!result) {
        CLOUD4RPI_PRINTLN("FAIL!");
    }
    return result;
}

void Cloud4RPi::mqttCallback(char* topic, byte* payload, unsigned int length) {
    String s = String(topic);
    CLOUD4RPI_PRINTLN();
    CLOUD4RPI_PRINT(s.substring(s.lastIndexOf("/")));
    CLOUD4RPI_PRINT(" --> ");
    for (int i = 0; i < length; i++) {
      CLOUD4RPI_PRINT((char)payload[i]);
    }
    CLOUD4RPI_PRINTLN();
    DynamicJsonBuffer json(this->jsonBufferSize);
    JsonObject& root = json.parseObject(payload);

    if(!root.success()) {
        CLOUD4RPI_PRINTLN("ERROR! Unable to parse message");
        return;
    }
    for(JsonObject::iterator item=root.begin(); item!=root.end(); ++item) {
        String key = item->key;
        bool value = item->value; // TODO other types!
        this->onCommand(key, value);
    }
}
void Cloud4RPi::onCommand(const String& command, bool value) {
    if (variables->canHandleCommand(command)) {
        bool newValue = variables->handleCommand<bool>(command, value);
        setVariable(command, newValue);
        publishData();
    } else {
        CLOUD4RPI_PRINT("No handler for '");
        CLOUD4RPI_PRINT(command);
        CLOUD4RPI_PRINTLN("' command.");
    }
}

void Cloud4RPi::printLogo() {
    CLOUD4RPI_PRINTLN();
    CLOUD4RPI_PRINTLN("_________ .__                   .___ _________________________.__ ");
    CLOUD4RPI_PRINTLN("\\_   ___ \\|  |   ____  __ __  __| _//  |  \\______   \\______   \\__|");
    CLOUD4RPI_PRINTLN("/    \\  \\/|  |  /  _ \\|  |  \\/ __ |/   |  ||       _/|     ___/  |");
    CLOUD4RPI_PRINTLN("\\     \\___|  |_(  <_> )  |  / /_/ /    ^   /    |   \\|    |   |  |");
    CLOUD4RPI_PRINTLN(" \\______  /____/\\____/|____/\\____ \\____   ||____|_  /|____|   |__|");
    CLOUD4RPI_PRINTLN("        \\/                       \\/    |__|       \\/");
}
