#pragma once
#include "JSValue.hpp"
#include <string>
#include <unordered_map>

class JSScope {
private:
  JSAllocator *_allocator;
  JSScope *_parent;
  JSAtom *_root;
  std::vector<JSScope *> _children;
  std::vector<JSValue *> _variables;
  std::unordered_map<std::wstring, JSValue *> _namedVariables;

public:
  JSScope(JSAllocator *allocator, JSScope *parent = nullptr);

  ~JSScope();

  inline JSAllocator *getAllocator() { return _allocator; }

  inline JSScope *getParent() { return _parent; }

  inline JSAtom *getRoot() { return _root; }

  void removeChild(JSScope *scope);

  JSAtom *createAtom(JSBase *data);

  JSValue *createValue(JSAtom *atom);

  JSValue *createValue(JSBase *val);

  JSValue *queryValue(const std::wstring &name);

  void storeValue(const std::wstring &name, JSValue *value);
};