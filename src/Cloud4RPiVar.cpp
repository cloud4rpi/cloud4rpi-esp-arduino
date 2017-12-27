#include "Cloud4RPiVar.h";

C4RVariableBase::C4RVariableBase(const String& _name) :
	name(_name) {
}

int C4RVariableList::count() {
  return itemCount;
}

C4RVariableBase* C4RVariableList::find(const String& _name) {
	ListNode* current = root;
	while(current) {
		C4RVariableBase* item = current->data;
		if (item && item->name == _name) {
			return item;
		}
		current = current->next;
	}
	return NULL;
}

int C4RVariableList::add(C4RVariableBase* _variable) {
	ListNode* node = new ListNode();
  node->next = root;
  node->data = _variable;
  root = node;
  itemCount++;
  return 0;
}
