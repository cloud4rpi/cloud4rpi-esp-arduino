#ifndef Cloud4RPiVar_h
#define Cloud4RPiVar_h
#endif

#include <stddef.h>
#include <Arduino.h>

// C4RVariableBase

class C4RVariableBase {
public:
	String name;
	C4RVariableBase(const String& _name);
};

// C4RVariable<T>

template<typename T>
class C4RVariable : public C4RVariableBase {
public:
	T value;
	C4RVariable(const String& _name, T _value);
};

template<typename T>
C4RVariable<T>::C4RVariable(const String& _name, T _value) :
	C4RVariableBase(_name),
	value(_value) {
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
    void declare(const String& varName) {
			C4RVariable<T>* item = new C4RVariable<T>(varName, T());
			list->add(item);
			C4RVariable<T>* var = (C4RVariable<T>*)list->find(varName);
		}
		template<typename T>
		T getValue(const String& varName) {
			C4RVariable<T>* var = (C4RVariable<T>*)list->find(varName);
			Serial.println("GET " + var->name + " " + String(var->value));
			return var->value;
		}

		template<typename T>
		void setValue(const String& varName, T _value) {
			C4RVariable<T>* item = (C4RVariable<T>*)list->find(varName);
			if (item) {
				Serial.println("SET " + item->name + " " + String(_value));
				item->value = _value;
			}
		}
};
