#pragma once

#include "JSBase.hpp"
#include <string>
class JSString : public JSBase {
private:
  std::wstring _value;

public:
  JSString(JSAllocator *allocator, const std::wstring &value);
  const std::wstring &getValue() const { return _value; }
  void setValue(const std::wstring &value) { _value = value; }
};
