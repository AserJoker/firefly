#include "script/engine/JSScope.hpp"
#include "script/engine/JSAtom.hpp"
#include <algorithm>
JSScope::JSScope(JSAllocator *allocator, JSScope *parent)
    : _allocator(allocator), _parent(parent) {
  if (_parent) {
    _parent->_children.push_back(this);
  }
  _root = _allocator->create<JSAtom>();
}

JSScope::~JSScope() {
  if (_parent) {
    auto it =
        std::find(_parent->_children.begin(), _parent->_children.end(), this);
    if (it != _parent->_children.end()) {
      _parent->_children.erase(it);
    }
  }
  _parent = nullptr;
  while (!_children.empty()) {
    _allocator->dispose(*_children.begin());
  }
  for (auto variable : _variables) {
    _allocator->dispose(variable);
  }
  _variables.clear();
  _allocator->dispose(_root);
  _root = nullptr;
  JSAtom::gc(_allocator);
}

void JSScope::removeChild(JSScope *scope) {
  auto it = std::find(_children.begin(), _children.end(), scope);
  if (it != _children.end()) {
    _children.erase(it);
  }
  scope->_parent = nullptr;
}
JSAtom *JSScope::createAtom(JSBase *data) {
  return _allocator->create<JSAtom>(_root, data);
}
JSValue *JSScope::createValue(JSAtom *atom) {
  _root->addChild(atom);
  JSValue *value = _allocator->create<JSValue>(atom);
  _variables.push_back(value);
  return value;
}
JSValue *JSScope::createValue(JSBase *val) {
  auto atom = createAtom(val);
  JSValue *value = _allocator->create<JSValue>(atom);
  _variables.push_back(value);
  return value;
}
JSValue *JSScope::queryValue(const std::wstring &name) {
  if (_namedVariables.contains(name)) {
    return _namedVariables.at(name);
  }
  return nullptr;
}
void JSScope::storeValue(const std::wstring &name, JSValue *value) {
  _namedVariables[name] = value;
}