#include "script/Script.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "script/Atom.hpp"
#include "script/Scope.hpp"
#include "script/Value.hpp"
#include <cstddef>
#include <list>
#include <unordered_map>
#include <vector>

using namespace firefly;
using namespace firefly::script;
Script::Script() {
  _root = new Scope();
  _current = _root;
  _bridge = core::Singleton<Bridge>::instance();
}
Script::~Script() {}
void Script::dispose() {
  if (_bridge != nullptr) {
    _bridge->dispose();
    _bridge = nullptr;
  }
  auto global = _root->getRoot();
  while (!global->_children.empty()) {
    auto it = *global->_children.begin();
    it->removeParent(global);
    destroy(it);
  }
  delete _root->getRoot();
  _root = nullptr;
}

void Script::setBridge(core::AutoPtr<Bridge> bridge) {
  if (_bridge != nullptr && _bridge != bridge) {
    _bridge->dispose();
  }
  _bridge = bridge;
  getNativeGlobal()->setField(this, "$objects", createValue()->setArray(this));
  getNativeGlobal()->setField(this, "$functions",
                              createValue()->setArray(this));
}
core::AutoPtr<Script::Bridge> Script::getBridge() { return _bridge; }
core::AutoPtr<Value> Script::getGlobal() {
  if (_bridge != nullptr) {
    return _bridge->getGlobal();
  }
  return getNativeGlobal();
}
core::AutoPtr<Value> Script::getNativeGlobal() {
  return createValue(_root->getRoot());
}
Value::Stack Script::eval(const std::string &source) {
  if (_bridge != nullptr) {
    return _bridge->eval(source);
  }
  return {};
}
core::AutoPtr<Value> Script::createValue(Atom *at) {
  Atom *atom = at;
  auto parent = _current->getRoot();
  if (!atom) {
    atom = new Atom();
    atom->_parent.push_back(parent);
    parent->_children.push_back(atom);
  } else {
    atom->_parent.push_back(parent);
    parent->_children.push_back(atom);
  }
  return new Value(atom);
}

core::AutoPtr<Value> Script::createValue(const AnyValue &value) {
  auto val = createValue();
  auto &type = value.value.type();
  if (type == typeid(double)) {
    val->setNumber(this, std::any_cast<double>(value.value));
  } else if (type == typeid(bool)) {
    val->setBoolean(this, std::any_cast<bool>(value.value));
  } else if (type == typeid(std::string)) {
    val->setString(this, std::any_cast<std::string>(value.value));
  } else if (type == typeid(Value::FunctionHandle)) {
    val->setFunction(this, std::any_cast<Value::FunctionHandle>(value.value));
  }
  return val;
}
core::AutoPtr<Value> Script::createValue(AnyValue &&value) {
  return createValue(value);
}
core::AutoPtr<Value> Script::createValue(const std::vector<AnyValue> &value) {
  auto arr = createValue()->setArray(this);
  for (auto i = 0; i < value.size(); i++) {
    arr->setIndex(this, i, createValue(value[i]));
  }
  return arr;
}
core::AutoPtr<Value>
Script::createValue(const std::unordered_map<std::string, AnyValue> &value) {
  auto obj = createValue()->setObject(this);
  for (auto &[key, field] : value) {
    obj->setField(this, key, createValue(field));
  }
  return obj;
}
core::AutoPtr<Scope> Script::pushScope() {
  core::AutoPtr<Scope> current = _current;
  _current = new Scope((Scope *)current.getRawPointer());
  return current;
}
void Script::popScope(core::AutoPtr<Scope> scope) {
  _current = scope;
  auto children = _current->getChildren();
  _current->getChildren().clear();
  for (auto &sco : children) {
    auto atom = sco->getRoot();
    atom->removeParent(_current->getRoot());
  }
  for (auto &sco : children) {
    destroy(sco->getRoot());
  }
}
core::AutoPtr<Scope> Script::getCurrentScope() { return _current; }
core::AutoPtr<Scope> Script::getRootScope() { return _root; }
void Script::store(const std::string &name, core::AutoPtr<Value> value) {
  _current->store(name, value);
}
core::AutoPtr<Value> Script::query(const std::string &name) {
  auto current = _current;
  core::AutoPtr<Value> value = nullptr;
  while (!value && current != nullptr) {
    value = current->load(name);
    current = current->getParent();
  }
  if (value == nullptr) {
    return createValue();
  }
  return value;
}

void Script::registerModule(const std::string &name,
                            core::AutoPtr<Value> exports) {
  if (_bridge != nullptr) {
    _bridge->registerModule(name, exports);
  }
}
void Script::gc() {
  if (_bridge != nullptr) {
    _bridge->gc();
  }
}

bool Script::isAlived(Atom *atom,
                      const std::unordered_map<ptrdiff_t, Atom *> &alived) {
  std::list<Atom *> workflow = {atom};
  std::unordered_map<ptrdiff_t, Atom *> cache;
  while (!workflow.empty()) {
    auto it = *workflow.begin();
    workflow.erase(workflow.begin());
    if (cache.contains((ptrdiff_t)it)) {
      continue;
    }
    cache[(ptrdiff_t)it] = it;
    if (alived.contains((ptrdiff_t)it) || it == _root->getRoot()) {
      return true;
    }
    auto parents = it->_parent;
    for (auto &p : parents) {
      workflow.push_back(p);
    }
  }
  return false;
}
void Script::destroy(Atom *atom) {
  if (_destroyList.contains((ptrdiff_t)atom) || atom == _root->getRoot()) {
    return;
  }
  static uint32_t destroyLevel = 0;
  destroyLevel++;
  auto root = _root->getRoot();
  std::unordered_map<ptrdiff_t, Atom *> cache;
  std::unordered_map<ptrdiff_t, Atom *> alived;
  std::list<Atom *> destroyed;
  std::list<Atom *> workflow = {atom};
  while (!workflow.empty()) {
    auto it = *workflow.begin();
    workflow.erase(workflow.begin());
    if (cache.contains((ptrdiff_t)it)) {
      continue;
    }
    cache[(ptrdiff_t)it] = it;
    if (isAlived(it, alived)) {
      alived[(ptrdiff_t)it] = it;
    } else {
      _destroyList[(ptrdiff_t)it] = it;
      destroyed.push_back(it);
    }
    for (auto &c : it->_children) {
      if (c == root) {
        c->removeParent(it);
      } else {
        workflow.push_back(c);
      }
    }
  }
  for (auto &c : destroyed) {
    if (c->_metadata) {
      auto scope = pushScope();
      auto metadata = createValue(c->_metadata);
      if (metadata->getType(this) != Atom::TYPE::NIL) {
        auto gc = metadata->getField(this, "gc");
        c->addParent(root);
        if (gc->getType(this) != Atom::TYPE::NIL) {
          auto obj = createValue(c);
          gc->call(this, {obj});
        }
      }
      popScope(scope);
      c->removeParent(root);
    }
  }
  if (destroyLevel == 1) {
    alived = {{(ptrdiff_t)root, root}};
    destroyed.clear();
    for (auto &[_, c] : _destroyList) {
      if (!isAlived(c, alived)) {
        while (!c->_children.empty()) {
          auto cc = *c->_children.begin();
          cc->removeParent(c);
        }
        while (!c->_parent.empty()) {
          auto p = *c->_parent.begin();
          c->removeParent(p);
        }
        destroyed.push_back(c);
      }
    }
    for (auto &c : destroyed) {
      delete c;
    }
    _destroyList.clear();
  }
  destroyLevel--;
}