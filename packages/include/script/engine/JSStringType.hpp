#pragma once
#include "JSType.hpp"
class JSStringType : public JSType {
public:
  JSStringType(JSAllocator *allocator);

public:
  const wchar_t *getTypeName() const override;

  JSValue *toString(JSContext *ctx, JSValue *value) const override;

  JSValue *toNumber(JSContext *ctx, JSValue *value) const override;

  JSValue *toBoolean(JSContext *ctx, JSValue *value) const override;

  JSValue *clone(JSContext *ctx, JSValue *value) const override;

  JSValue *pack(JSContext *ctx, JSValue *value) const override;

  JSValue *equal(JSContext *ctx, JSValue *value,
                 JSValue *another) const override;
                 
  virtual JSValue *add(JSContext *ctx, JSValue *value, JSValue *another) const;
};