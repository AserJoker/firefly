#pragma once
#include "script/engine/JSObjectType.hpp"
#include "script/engine/JSType.hpp"
class JSCallableType : public JSObjectType {
public:
  JSCallableType(JSAllocator *allocator);

public:
  const wchar_t *getTypeName() const override;

public:
  virtual JSValue *call(JSContext *ctx, JSValue *func, JSValue *self,
                        std::vector<JSValue *> args) const = 0;
};