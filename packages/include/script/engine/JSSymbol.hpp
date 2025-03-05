#pragma once
#include "JSBase.hpp"
#include <string>
class JSSymbol : public JSBase {
private:
  std::wstring _description;

public:
  JSSymbol(JSAllocator *allocator, const std::wstring &description);

  const std::wstring &getDescription() const { return _description; }
};