#pragma once
#include "script/engine/JSContext.hpp"
#include "script/engine/JSObjectType.hpp"
#include "script/engine/JSValue.hpp"
#include "script/util/JSAllocator.hpp"
class JSArrayType : public JSObjectType {
public:
  JSArrayType(JSAllocator *allocator);

public:
  JSValue *toString(JSContext *ctx, JSValue *value) const override;

  JSValue *getField(JSContext *ctx, JSValue *array,
                    JSValue *name) const override;
                    
  JSValue *setField(JSContext *ctx, JSValue *array, JSValue *name,
                    JSValue *value) const override;
};