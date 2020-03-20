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
    variables(new C4RVariableStorage()),
    diagnostics(new C4RVariableStorage()),
    jsonBufferSize(JSON_BUFFER_SIZE) {

}

Cloud4RPi::~Cloud4RPi() {
    if (mqttClient != NULL) {
        mqttClient = NULL;
    }

    if (variables != NULL) {
        delete variables;
        variables = NULL;
    }

    if (diagnostics != NULL) {
        delete diagnostics;
        diagnostics = NULL;
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

void Cloud4RPi::declareBoolVariable(const String& varName, C4R_BOOL_HANDLER_SIGNATURE) {
    if (!isVariableExists(varName)) {
        variables->declare<bool>(varName, VAR_TYPE_BOOL, cmdHandler);
     }
}

void Cloud4RPi::declareNumericVariable(const String& varName, C4R_NUMERIC_HANDLER_SIGNATURE) {
    if (!isVariableExists(varName)) {
        variables->declare<double>(varName, VAR_TYPE_NUMERIC, cmdHandler);
    }
}

void Cloud4RPi::declareStringVariable(const String& varName) {
    if (!isVariableExists(varName)) {
        variables->declare<char*>(varName, VAR_TYPE_STRING);
    }
}

void Cloud4RPi::declareDiagVariable(const String& varName) {
    if (!isVariableExists(varName, true)) {
        diagnostics->declare<char*>(varName, VAR_TYPE_STRING);
    }
}

bool Cloud4RPi::isVariableExists(const String& varName, bool isDiag) {
    bool exists = isDiag ? diagnostics->exists(varName) : variables->exists(varName);
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
    return variables->getValue<String>(varName);
}

String Cloud4RPi::getDiagValue(const String& varName) {
    return diagnostics->getValue<String>(varName);
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

void Cloud4RPi::setVariable(const String& varName, const char* value) {
    variables->setValue(varName, String(value));
}

void Cloud4RPi::setVariable(const String& varName, const String& value) {
    variables->setValue(varName, value);
}


void Cloud4RPi::setDiagVariable(const String& varName, const char* value) {
    diagnostics->setValue(varName, String(value));
}
void Cloud4RPi::setDiagVariable(const String& varName, const String& value) {
    diagnostics->setValue(varName, value);
}

bool Cloud4RPi::publishConfig() {
    DynamicJsonDocument doc(jsonBufferSize);
    JsonObject root = doc.to<JsonObject>();
    root["v"] = VERSION;
    root["l"] = LANG;
    JsonArray payload = root.createNestedArray("payload");

    for(int i = 0; i < variables->size(); i++) {
        JsonObject item = payload.createNestedObject();
        C4RVariableInfo info = variables->getVariableInfo(i);
        item["name"] = info.name;
        item["type"] = info.type;
    }
    return this->publishCore(doc, "/config");
}

bool Cloud4RPi::publishData() {
    DynamicJsonDocument doc(jsonBufferSize);
    JsonObject root = doc.to<JsonObject>();
    JsonObject payload = root.createNestedObject("payload");

    for(int i = 0; i < variables->size(); i++) {
        C4RVariableInfo info = variables->getVariableInfo(i);
        if(info.type == VAR_TYPE_BOOL) {
          payload[info.name] = this->getBoolValue(info.name);
        };
        if(info.type == VAR_TYPE_NUMERIC) {
          payload[info.name] = this->getNumericValue(info.name);
        };
        if(info.type == VAR_TYPE_STRING) {
            payload[info.name] = this->getStringValue(info.name);
        };
    }
    return this->publishCore(doc, "/data");
}

bool Cloud4RPi::publishDiag() {
    DynamicJsonDocument doc(jsonBufferSize);
    JsonObject payload = doc.createNestedObject("payload");

    for(int i = 0; i < diagnostics->size(); i++) {
        C4RVariableInfo info = diagnostics->getVariableInfo(i);
        payload[info.name] = this->getDiagValue(info.name);
    }
    return this->publishCore(doc, "/diagnostics");
}

bool Cloud4RPi::publishCore(DynamicJsonDocument doc, const String& subTopic) {
    if(!this->connected()) {
        return false;
    }
    JsonObject root = doc.as<JsonObject>();
    String topic = "devices/" + deviceToken + subTopic;
    // TODO impl
    // if(currentTime.length() > 0) root["ts"] = currentTime.c_str();
    int length = measureJson(root) + 1;
    char buffer[length];
    serializeJson(root, buffer, length);
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
    for (unsigned int i = 0; i < length; i++) {
      CLOUD4RPI_PRINT((char)payload[i]);
    }
    CLOUD4RPI_PRINTLN();
    DynamicJsonDocument doc(this->jsonBufferSize);
    deserializeJson(doc, payload);
    if(doc.isNull()) {
        CLOUD4RPI_PRINTLN("ERROR! Unable to parse message");
        return;
    }
    JsonObject root = doc.as<JsonObject>();
    for(JsonObject::iterator item=root.begin(); item!=root.end(); ++item) {
        this->onCommand(item->key().c_str(), item->value());
    }
}

void Cloud4RPi::onCommand(const String& command, JsonVariant value) {
    C4RVariableBase* item = variables->find(command);
    if (!item) {
        CLOUD4RPI_PRINT("Variable ''");
        CLOUD4RPI_PRINT(command);
        CLOUD4RPI_PRINTLN("' not found.");
        return;
    }
    if (!item->hasHandler()) {
        CLOUD4RPI_PRINT("No handler for '");
        CLOUD4RPI_PRINT(command);
        CLOUD4RPI_PRINTLN("' command.");
        return;
    }

    DynamicJsonDocument doc(jsonBufferSize);
    JsonObject root = doc.to<JsonObject>();
    JsonObject payload = root.createNestedObject("payload");

    String type = item->getType();
    if(type == VAR_TYPE_BOOL) {
        variables->handleCommand<bool>(command, value.as<bool>());
        payload[command] = this->getBoolValue(command);
    }
    if(type == VAR_TYPE_NUMERIC) {
        variables->handleCommand<double>(command, value.as<double>());
        payload[command] = this->getNumericValue(command);
    }
    this->publishCore(doc, "/data/cr");
}

String Cloud4RPi::getVersion() {
    return VERSION;
}

void Cloud4RPi::printLogo() {
    CLOUD4RPI_PRINTER.println();
    CLOUD4RPI_PRINTER.println("_________ .__                   .___ _________________________.__ ");
    CLOUD4RPI_PRINTER.println("\\_   ___ \\|  |   ____  __ __  __| _//  |  \\______   \\______   \\__|");
    CLOUD4RPI_PRINTER.println("/    \\  \\/|  |  /  _ \\|  |  \\/ __ |/   |  ||       _/|     ___/  |");
    CLOUD4RPI_PRINTER.println("\\     \\___|  |_(  <_> )  |  / /_/ /    ^   /    |   \\|    |   |  |");
    CLOUD4RPI_PRINTER.println(" \\______  /____/\\____/|____/\\____ \\____   ||____|_  /|____|   |__|");
    CLOUD4RPI_PRINTER.println("        \\/                       \\/    |__|       \\/");
}
