#include "script/Context.hpp"
#include "core/AutoPtr.hpp"
#include "script/Atom.hpp"
#include "script/Scope.hpp"
#include "script/Value.hpp"
#include <cstddef>
#include <queue>
#include <unordered_map>
#include <vector>
using namespace firefly;
using namespace firefly::script;
Context::Context(core::AutoPtr<Bridge> bridge, core::AutoPtr<Scope> scope)
    : _bridge(bridge), _root(scope) {
  if (_root == nullptr) {
    _root = new Scope();
  }
  _current = _root;
}
Context::~Context() {
  if (_root != nullptr) {
    core::AutoPtr<Context> ctx = new Context(_bridge, _root);
    gc(ctx, _root->getRoot());
    ctx->_root = nullptr;
  }
}
core::AutoPtr<Context::Bridge> Context::getBridge() { return _bridge; }
core::AutoPtr<Value> Context::getGlobal() {
  return new Value(_root->getRoot());
}
core::AutoPtr<Value> Context::eval(const std::string &filename,
                                   const std::string &source) {
  return nullptr;
}
core::AutoPtr<Value> Context::createValue(Atom *at) {
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
core::AutoPtr<Scope> Context::pushScope() {
  core::AutoPtr<Scope> current = _current;
  _current = new Scope(&*current);
  return current;
}
void Context::popScope(core::AutoPtr<Scope> scope) {
  _current = scope;
  for (auto &sco : _current->getChildren()) {
    auto atom = sco->getRoot();
    gc(this, atom);
  }
  _current->getChildren().clear();
}
core::AutoPtr<Scope> Context::getCurrentScope() { return _current; }
core::AutoPtr<Scope> Context::getRootScope() { return _root; }
bool Context::checkAlived(Atom *atom,
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
void Context::gc(core::AutoPtr<Context> ctx, Atom *atom) {
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