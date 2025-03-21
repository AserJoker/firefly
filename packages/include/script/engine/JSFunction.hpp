#pragma once
#include "JSCallable.hpp"
#include "script/engine/JSType.hpp"
#include <string>
class JSFunction : public JSCallable {
private:
  std::wstring _path;

  size_t _address;

public:
  JSFunction(JSAllocator *allocator, const std::wstring &name = L"",
             const std::wstring &path = L"", size_t address = 0,
             const std::unordered_map<std::wstring, JSAtom *> &closure = {},
             JSType *type = nullptr);

  inline const std::wstring &getProgramPath() const { return _path; }

  inline size_t getAddress() const { return _address; }

  void setProgram(const std::wstring &path, size_t address) {
    _path = path;
    _address = address;
  }
};