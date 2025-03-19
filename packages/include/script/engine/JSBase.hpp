#pragma once
#include "../util/JSAllocator.hpp"
#include "JSType.hpp"
#include "script/util/JSRef.hpp"
#include <string>
#include <unordered_map>
class JSAtom;
class JSBase : public JSRef {
private:
  JSType *_type;
  std::unordered_map<std::wstring, JSAtom *> _metadata;

public:
  JSBase(JSAllocator *allocator, JSType *type);

  virtual ~JSBase();

  inline const JSType *getType() const { return _type; }

  template <class T> T *cast() { return dynamic_cast<T *>(this); }

  template <class T> bool isTypeof() const {
    return getType()->cast<T>() != nullptr;
  }
  
  inline const std::unordered_map<std::wstring, JSAtom *> &getMetadata() const {
    return _metadata;
  }

  inline std::unordered_map<std::wstring, JSAtom *> &getMetadata() {
    return _metadata;
  }
};