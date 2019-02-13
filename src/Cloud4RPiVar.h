#ifndef _CLOUD4RPIVAR_H
#define _CLOUD4RPIVAR_H

#include <stddef.h>
#include <Arduino.h>

#if defined(ESP8266) || defined(ESP32)
#include <functional>
#define C4R_BOOL_HANDLER_SIGNATURE std::function<bool(bool)> cmdHandler
#define C4R_NUMERIC_HANDLER_SIGNATURE std::function<double(double)> cmdHandler
#define C4R_T_HANDLER_SIGNATURE std::function<T(T)> cmdHandler
#else
#define C4R_BOOL_HANDLER_SIGNATURE bool (*cmdHandler)(bool)
#define C4R_NUMERIC_HANDLER_SIGNATURE double (*cmdHandler)(double)
#define C4R_T_HANDLER_SIGNATURE T (*cmdHandler)(T)
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
    virtual bool hasHandler();
};

// C4RVariable<T>

template<typename T>
class C4RVariable : public C4RVariableBase {
private:
  String type;
  T value;
  C4R_T_HANDLER_SIGNATURE;
public:
    C4RVariable(const String& _name, const String& _type, T _value) :
        C4RVariableBase(_name),
        type(_type),
        value(_value) {
    };

    virtual String getType() { return type; };
    T getValue() { return value; };
    void setValue(T _value) { value = _value; }
    bool hasHandler() {  return cmdHandler != NULL; };
    void setHandler(C4R_T_HANDLER_SIGNATURE) { this->cmdHandler = cmdHandler; }
    void handleCommand(T value) {
        if (cmdHandler) {
            T newValue = cmdHandler(value);
            setValue(newValue);
        }
    }
};

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
    void declare(const String& varName, const String& varType, C4R_T_HANDLER_SIGNATURE = NULL) {
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
        return var->getValue();
    }

    template<typename T>
    void setValue(const String& varName, T _value) {
        C4RVariable<T>* var = (C4RVariable<T>*)list->find(varName);
        if (var) {
            var->setValue(_value);
        }
    }

    C4RVariableBase* find(const String& name) {
        return list->find(name);
    }

    template<typename T>
    void handleCommand(const String& command, T _value) { // optimize double find
        C4RVariable<T>* var = (C4RVariable<T>*)list->find(command);
        var->handleCommand(_value);
    }
};
#endif // _CLOUD4RPIVAR_H
