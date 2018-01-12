#ifndef Cloud4RPiVar_h
#define Cloud4RPiVar_h

#include <stddef.h>
#include <Arduino.h>

#ifdef ESP8266
#include <functional>
#define C4R_HANDLER_SIGNATURE std::function<bool(bool)> cmdHandler
#else
#define C4R_HANDLER_SIGNATURE bool (*cmdHandler)(bool)
#endif


struct C4RVariableInfo {
    String name;
    String type;
};

// C4RVariableBase

class C4RVariableBase {
private:
  String name;
public:
    C4RVariableBase(const String& _name);

    String getName();
    virtual String getType() = 0;
    virtual bool hasHandler() { return false; };
};

// C4RVariable<T>

template<typename T>
class C4RVariable : public C4RVariableBase {
private:
  String type;
  T value;
  C4R_HANDLER_SIGNATURE;
public:
    C4RVariable(const String& _name, const String& _type, T _value);
    virtual String getType() { return type; };
    T getValue();
    void setValue(T _value);
    virtual bool hasHandler() { return this->cmdHandler != NULL; };

    void setHandler(C4R_HANDLER_SIGNATURE) {
        this->cmdHandler = cmdHandler;
    }

    T handleCommand(T value) {
        if (cmdHandler) {
            return cmdHandler(value);
        }
        return NULL;
    }
};

template<typename T>
C4RVariable<T>::C4RVariable(const String& _name, const String& _type, T _value) :
    C4RVariableBase(_name),
    type(_type),
    value(_value) {
};

template<typename T>
void C4RVariable<T>::setValue(T _value) {
  value = _value;
}

template<typename T>
T C4RVariable<T>::getValue() { return value; };


// C4RVariableList

class C4RVariableList {
    struct ListNode {
        C4RVariableBase* data;
        ListNode* next;
    };
private:
    unsigned int count = 0;
    ListNode* root;
public:
    ~C4RVariableList();
    int size();
    void add(C4RVariableBase* _variable);
    C4RVariableBase* get(int index);
    C4RVariableBase* find(const String& name);
};

// C4RVariableStorage

class C4RVariableStorage {
private:
    C4RVariableList* list;
public:
    C4RVariableStorage();
    ~C4RVariableStorage();

    int size();
    bool exists(const String& varName);

    template<typename T>
    void declare(const String& varName, const String& varType, C4R_HANDLER_SIGNATURE = NULL) {
        C4RVariable<T>* item = new C4RVariable<T>(varName, varType, T());
        item->setHandler(cmdHandler);
        list->add(item);
    }

    C4RVariableInfo getVariableInfo(int index) {
        C4RVariableBase* var = list->get(index);
        return { name: var->getName(), type: var->getType() };
    }

    template<typename T>
    T getValue(const String& varName) {
        C4RVariable<T>* var = (C4RVariable<T>*)list->find(varName);
        //Serial.println("GET " + var->getName() + " " + String(var->getValue()));
        return var->getValue();
    }

    template<typename T>
    void setValue(const String& varName, T _value) {
        C4RVariable<T>* var = (C4RVariable<T>*)list->find(varName);
        if (var) {
            //Serial.println("SET " + var->getName() + " " + String(_value));
            var->setValue(_value);
        }
    }

    bool canHandleCommand(const String& command) {
        C4RVariableBase* var = list->find(command);
        return var && var->hasHandler();
    }

    template<typename T>
    T handleCommand(const String& command, T _value) { // optimize double find
        C4RVariable<T>* var = (C4RVariable<T>*)list->find(command);
        return var->handleCommand(_value);
    }
};
#endif
