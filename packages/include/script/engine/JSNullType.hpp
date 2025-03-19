#pragma once
#include "JSType.hpp"
class JSNullType : public JSType {
public:
  JSNullType(JSAllocator *allocator);

public:
  const wchar_t *getTypeName() const override;

  JSValue *toString(JSContext *ctx, JSValue *value) const override;

  JSValue *toNumber(JSContext *ctx, JSValue *value) const override;

  JSValue *toBoolean(JSContext *ctx, JSValue *value) const override;

  JSValue *clone(JSContext *ctx, JSValue *value) const override;

  JSValue *pack(JSContext *ctx, JSValue *value) const override;

  JSValue *equal(JSContext *ctx, JSValue *value,
                 JSValue *another) const override;
};