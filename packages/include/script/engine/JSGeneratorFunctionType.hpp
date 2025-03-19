#pragma once
#include "script/engine/JSFunctionType.hpp"
class JSGeneratorFunctionType : public JSFunctionType {
public:
  JSGeneratorFunctionType(JSAllocator *allocator);
  JSValue *call(JSContext *ctx, JSValue *func, JSValue *self,
                std::vector<JSValue *> args) const override;
};