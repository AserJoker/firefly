#pragma once
#include "JSBase.hpp"
class JSSymbol : public JSBase {
private:
  std::wstring _description;

public:
  JSSymbol(JSAllocator *allocator, const std::wstring &description)
      : JSBase(allocator, JS_TYPE::SYMBOL), _description(description) {}

  const std::wstring &getDescription() const { return _description; }

public:
  JSBase *toString() override;
  JSBase *clone() override;
};