#pragma once
#include "JSAtom.hpp"
#include "JSType.hpp"
class JSValue {
private:
  JSAllocator *_allocator;
  JSAtom *_atom;

  bool _const;

public:
  JSValue(JSAllocator *allocator, JSAtom *atom)
      : _allocator(allocator), _atom(atom), _const(false) {}

  ~JSValue() {
    _allocator = nullptr;
    _atom = nullptr;
  }

  JSAllocator *getAllocator() { return _allocator; }

  inline auto getAtom() { return _atom; }

  inline auto getAtom() const { return _atom; }

  inline void setAtom(JSAtom *atom) { _atom = atom; }

  inline auto getData() { return _atom->getData(); }

  inline auto getData() const { return _atom->getData(); }

  inline auto getType() const { return _atom->getType(); }

  inline auto isConst() const { return _const; }

  inline void setConst(bool value) { _const = value; }

  template <class T> bool isTypeof() const { return _atom->isTypeof<T>(); }
};