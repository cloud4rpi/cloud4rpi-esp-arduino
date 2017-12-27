#include "Cloud4RPiVar.h";

C4RVariableBase::C4RVariableBase(const char* _name) :
	name(_name) {
}

int C4RVariableList::count() {
  return itemCount;
}

C4RVariableBase* C4RVariableList::find(const char* name) {
	C4RVariableListNode* current = root;
	while(current) {
		C4RVariableBase* item = current->data;
		if (item && strcmp(item->name, name) == 0) {
			return item;
		}
		current = current->next;
	}
	return NULL;
}

int C4RVariableList::add(C4RVariableBase* _variable) {
	C4RVariableListNode* node = new C4RVariableListNode();
  node->next = root;
  node->data = _variable;
  root = node;
  itemCount++;
  return 0;
}
