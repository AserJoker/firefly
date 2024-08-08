#pragma once
#include <algorithm>
#include <any>
#include <fmt/core.h>
#include <vector>
namespace firefly::script {
struct Atom {
  enum class Type { NIL = 0, NUMBER, BOOLEAN, STRING, OBJECT, ARRAY, FUNCTION };

  std::vector<Atom *> _parent;
  std::vector<Atom *> _children;
  Atom *_metadata;
  std::any _value;
  Type _type;
  bool _marked;
  bool _disposed;
  Atom() {
    _type = Type::NIL;
    _value = nullptr;
    _metadata = nullptr;
    _marked = false;
    _disposed = false;
  }
  void addParent(Atom *parent) {
    parent->_children.push_back(this);
    _parent.push_back(parent);
  }
  void removeParent(Atom *parent) {
    auto it = std::find(_parent.begin(), _parent.end(), parent);
    _parent.erase(it);
    it = std::find(parent->_children.begin(), parent->_children.end(), this);
    parent->_children.erase(it);
  }
};
}; // namespace firefly::script