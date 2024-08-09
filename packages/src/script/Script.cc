#include "script/Script.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "script/Atom.hpp"
#include "script/Scope.hpp"
#include "script/Value.hpp"
#include <cstddef>
#include <queue>
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
  _bridge = nullptr;
  gc(this, _root->getRoot());
  _root = nullptr;
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
Value::Stack Script::eval(const std::string &filename,
                          const std::string &source) {
  if (_bridge != nullptr) {
    return _bridge->eval(filename, source);
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
core::AutoPtr<Scope> Script::pushScope() {
  core::AutoPtr<Scope> current = _current;
  _current = new Scope(&*current);
  return current;
}
void Script::popScope(core::AutoPtr<Scope> scope) {
  _current = scope;
  for (auto &sco : _current->getChildren()) {
    auto atom = sco->getRoot();
    gc(this, atom);
  }
  _current->getChildren().clear();
}
core::AutoPtr<Scope> Script::getCurrentScope() { return _current; }
core::AutoPtr<Scope> Script::getRootScope() { return _root; }
bool Script::checkAlived(Atom *atom,
                         const std::unordered_map<ptrdiff_t, Atom *> &alived) {
  if (atom->_disposed) {
    return false;
  }
  std::queue<Atom *> workflow;
  workflow.push(atom);
  std::unordered_map<ptrdiff_t, Atom *> cache;
  while (!workflow.empty()) {
    auto item = workflow.front();
    workflow.pop();
    if (alived.contains((ptrdiff_t)item)) {
      return true;
    }
    if (cache.contains((ptrdiff_t)item) || item->_disposed) {
      continue;
    }
    cache[(ptrdiff_t)item] = item;
    for (auto &p : item->_parent) {
      workflow.push(p);
    }
  }
  return false;
}
void Script::gc(core::AutoPtr<Script> ctx, Atom *atom) {
  static int level = 0;
  level++;
  atom->_disposed = true;
  std::unordered_map<ptrdiff_t, Atom *> alived;
  std::unordered_map<ptrdiff_t, Atom *> destroyed;
  std::queue<Atom *> workflow;
  workflow.push(atom);
  if (atom != ctx->_root->getRoot()) {
    auto root = ctx->_root->getRoot();
    alived[(ptrdiff_t)root] = root;
  }
  while (!workflow.empty()) {
    auto item = workflow.front();
    workflow.pop();
    auto addr = (ptrdiff_t)item;
    if (alived.contains(addr) || destroyed.contains(addr)) {
      continue;
    }
    if (checkAlived(item, alived)) {
      item->_marked = false;
      alived[(ptrdiff_t)item] = item;
    } else {
      if (!item->_marked) {
        item->_marked = true;
        destroyed[(ptrdiff_t)item] = item;
        for (auto &c : item->_children) {
          workflow.push(c);
        }
      }
    }
  }
  for (auto &[_, item] : destroyed) {
    if (item->_metadata) {
      auto metadata = ctx->createValue(item->_metadata);
      auto gc = metadata->getField(ctx, "gc");
      if (gc->getType(ctx) != Atom::Type::NIL) {
        gc->call(ctx, {new Value(item)});
      }
    }
  }
  level--;
  if (level == 0) {
    workflow.push(atom);
    std::unordered_map<ptrdiff_t, Atom *> cache;
    destroyed.clear();
    while (!workflow.empty()) {
      auto item = workflow.front();
      workflow.pop();
      if (cache.contains((ptrdiff_t)item)) {
        continue;
      }
      if (item->_marked || atom == ctx->_root->getRoot()) {
        for (auto &c : item->_children) {
          std::erase(c->_parent, item);
          workflow.push(c);
        }
        for (auto &p : item->_parent) {
          std::erase(p->_children, item);
        }
        destroyed[(ptrdiff_t)item] = item;
      }
      cache[(ptrdiff_t)item] = item;
    }
    for (auto &[_, item] : destroyed) {
      delete item;
    }
  }
}
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

void Script::registerModule(
    const std::string &name,
    std::unordered_map<std::string, core::AutoPtr<Value>> exports) {
  if (_bridge != nullptr) {
    _bridge->registerModule(name, exports);
  }
}