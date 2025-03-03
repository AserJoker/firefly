#pragma once

#include "JSBase.hpp"
class JSString : public JSBase {
private:
  std::wstring _value;

public:
  JSString(JSAllocator *allocator, const std::wstring &value)
      : JSBase(allocator, JS_TYPE::STRING), _value(value) {}
  const std::wstring &getValue() const { return _value; }
  void setValue(const std::wstring &value) { _value = value; }

public:
  JSBase *toString() override;
  JSBase *clone() override;
};
