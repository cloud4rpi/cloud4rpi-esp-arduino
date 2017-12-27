#ifndef Cloud4RPiVar_h
#define Cloud4RPiVar_h
#endif

#include <stddef.h>
#include <Arduino.h>

class C4RVariableBase {
public:
	String name;
	C4RVariableBase(const String& _name);
};

template<typename T>
class C4RVariable : public C4RVariableBase {
public:
	T value;
	C4RVariable(const String& _name, T _value);
};

// NOTE: do not move to .cpp
template<typename T>
C4RVariable<T>::C4RVariable(const String& _name, T _value) :
	C4RVariableBase(_name),
	value(_value) {
};

class C4RVariableList {
	struct ListNode {
		C4RVariableBase* data;
		ListNode* next;
	};
private:
	unsigned int itemCount = 0;
 	ListNode* root;
	ListNode* last;
public:
	int count();
	int add(C4RVariableBase* _variable);
	C4RVariableBase* find(const String& name);
};
