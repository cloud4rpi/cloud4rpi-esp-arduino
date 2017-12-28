#include "Cloud4RPiVar.h"

C4RVariableBase::C4RVariableBase(const String& _name) :
	name(_name) {
}

// C4RVariableList

C4RVariableList::~C4RVariableList() {
  ListNode* node;
  while(root != NULL) {
    node = root;
    root = root->next;
    delete node;
  }
  count = 0;
}

int C4RVariableList::size() {
  return count;
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

void C4RVariableList::add(C4RVariableBase* _variable) {
	ListNode* node = new ListNode();
  node->next = root;
  node->data = _variable;
  root = node;
  count++;
}

// C4RVariableStorage

C4RVariableStorage::C4RVariableStorage() :
  list(new C4RVariableList()) {
}

C4RVariableStorage::~C4RVariableStorage() {
  if (list != NULL) {
    delete list;
    list = NULL;
  }
}

int C4RVariableStorage::size() {
  return list->size();
}

bool C4RVariableStorage::exists(const String& varName) {
  return list->find(varName) != NULL;
}
