#pragma once
#include "JSCallable.hpp"
class JSFunction : public JSCallable {
private:
  std::wstring _path;
  size_t _address;

public:
  JSFunction(JSAllocator *allocator, const std::wstring &name,
             const std::wstring &path, size_t address,
             const std::unordered_map<std::wstring, JSAtom *> &closure);
             
  JSValue *call(JSContext *ctx, JSValue *self,
                const std::vector<JSValue *> args) override;
};