#pragma once
#include "script/engine/JSBase.hpp"
class JSInfinity : public JSBase {
private:
  bool _negative;

public:
  JSInfinity(JSAllocator *allocator, bool negative = false);

  bool isNegative() const;
};