#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <algorithm>
#include <any>
#include <fmt/core.h>
#include <vector>
namespace firefly::script {
struct Atom {
  enum class TYPE { NIL = 0, NUMBER, BOOLEAN, STRING, OBJECT, ARRAY, FUNCTION };

  std::vector<Atom *> _parent;
  std::vector<Atom *> _children;
  core::AutoPtr<core::Object> _opaque;
  Atom *_metadata;
  std::any _value;
  TYPE _type;
  Atom() {
    _type = TYPE::NIL;
    _value = nullptr;
    _metadata = nullptr;
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