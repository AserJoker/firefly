#pragma once
#include "JSCallableType.hpp"
class JSFunctionType : public JSCallableType {
public:
  JSFunctionType(JSAllocator *allocator);

public:
  JSValue *call(JSContext *ctx, JSValue *func, JSValue *self,
                std::vector<JSValue *> args) const override;
};