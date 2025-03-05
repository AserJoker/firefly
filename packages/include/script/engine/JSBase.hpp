#pragma once
#include "../util/JSAllocator.hpp"
#include "JSType.hpp"
#include "script/util/JSRef.hpp"
#include "script/util/JSSingleton.hpp"
class JSBase : public JSRef {
private:
  JSType *_type;

public:
  JSBase(JSAllocator *allocator, JSType *type);

  virtual ~JSBase();

  inline const JSType *getType() const { return _type; }

  template <class T> T *cast() { return dynamic_cast<T *>(this); }

  template <class T> bool isTypeof() const {
    return getType() == (const JSType *)JSSingleton::query<T>();
  }
};