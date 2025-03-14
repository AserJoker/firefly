#pragma once
#include "JSBase.hpp"
#include "script/util/BigInt.hpp"
class JSBigInt : public JSBase {
private:
  BigInt<> _value;

public:
  JSBigInt(JSAllocator *allocator, const BigInt<> &value);
  
  inline const BigInt<> &getValue() const { return _value; }

  inline void setValue(const BigInt<> &value) { _value = value; }
};