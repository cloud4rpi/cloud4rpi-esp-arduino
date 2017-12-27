#ifndef Cloud4RPiVar_h
#define Cloud4RPiVar_h
#endif

#include <stddef.h>
#include <string.h>

class C4RVariableBase {
public:
	const char* name;
	C4RVariableBase(const char* _name);
};

template<typename T>
class C4RVariable : public C4RVariableBase {
public:
	T value;
	C4RVariable(const char* _name, T _value);
};

// NOTE: do not move to .cpp
template<typename T>
C4RVariable<T>::C4RVariable(const char* _name, T _value) :
	C4RVariableBase(_name),
	value(_value) {
};

class C4RVariableList {
	struct C4RVariableListNode {
		C4RVariableBase* data;
		C4RVariableListNode* next;
	};
private:
	unsigned int itemCount = 0;
 	C4RVariableListNode* root;
	C4RVariableListNode* last;
public:
	int add(C4RVariableBase* _variable);
	C4RVariableBase* find(const char* name);
	int count();
};
