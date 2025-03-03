#pragma once
#include "JSAtom.hpp"
#include "JSBase.hpp"
#include "JSType.hpp"
class JSValue {
private:
  JSAllocator *_allocator;
  JSAtom *_atom;

  bool _const;

public:
  JSValue(JSAllocator *allocator, JSAtom *atom)
      : _allocator(allocator), _atom(atom), _const(false) {}

  ~JSValue() {}

  JSAllocator *getAllocator() { return _allocator; }

  JSAtom *getAtom() { return _atom; }

  const JSAtom *getAtom() const { return _atom; }

  void setAtom(JSAtom *atom) { _atom = atom; }

  JSBase *getData() { return _atom->getData(); }

  const JSBase *getData() const { return _atom->getData(); }

  const JS_TYPE &getType() const { return _atom->getType(); }

  bool isConst() const { return _const; }

  void setConst(bool value) { _const = value; }
};