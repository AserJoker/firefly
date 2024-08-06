#pragma once
#include <any>
#include <fmt/core.h>
#include <iostream>
#include <vector>
namespace firefly::script {
struct Atom {
  enum class Type {
    NIL = 0,
    NUMBER = 1,
    INTEGER = 2,
    BOOLEAN = 3,
    STRING = 4,
    OBJECT = 5,
    ARRAY = 6,
    FUNCTION = 7
  };

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
};
}; // namespace firefly::script