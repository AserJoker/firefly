#pragma once
#include "JSCallable.hpp"
#include <string>
class JSFunction : public JSCallable {
private:
  std::wstring _path;
  size_t _address;

public:
  JSFunction(JSAllocator *allocator, const std::wstring &name,
             const std::wstring &path, size_t address,
             const std::unordered_map<std::wstring, JSAtom *> &closure);

  inline const std::wstring &getProgramPath() const { return _path; }
  
  inline size_t getAddress() const { return _address; }
};