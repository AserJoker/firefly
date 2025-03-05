#pragma once
#include "JSCallableType.hpp"
class JSNativeFunctionType : public JSCallableType {
public:
  JSNativeFunctionType(JSAllocator *allocator);

public:
  JSValue *call(JSContext *ctx, JSValue *func, JSValue *self,
                std::vector<JSValue *> args) const override;
};