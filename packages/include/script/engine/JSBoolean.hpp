#pragma once
#include "JSBase.hpp"
class JSBoolean : public JSBase {
private:
  bool _value;

public:
  JSBoolean(JSAllocator *allocator, bool value);

  inline bool getValue() const { return _value; }
  
  inline void setValue(bool value) { _value = value; }
};