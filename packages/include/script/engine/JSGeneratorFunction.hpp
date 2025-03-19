#pragma once
#include "script/engine/JSFunction.hpp"
class JSGeneratorFunction : public JSFunction {
public:
  JSGeneratorFunction(
      JSAllocator *allocator, const std::wstring &name = L"",
      const std::wstring &path = L"", size_t address = 0,
      const std::unordered_map<std::wstring, JSAtom *> &closure = {});
};