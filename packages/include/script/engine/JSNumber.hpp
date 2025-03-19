#pragma once
#include "JSBase.hpp"
class JSNumber : public JSBase {
private:
  double _value;

public:
  JSNumber(JSAllocator *allocator, double value);
  inline double getValue() const { return _value; }
  inline void setValue(double value) { _value = value; }
};