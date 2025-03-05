#pragma once
#include "JSType.hpp"
#include "script/engine/JSContext.hpp"
class JSObjectType : public JSType {
public:
  JSObjectType(JSAllocator *allocator);

public:
  const wchar_t *getTypeName() const override;

  JSValue *toString(JSContext *ctx, JSValue *value) const override;

  JSValue *toNumber(JSContext *ctx, JSValue *value) const override;

  JSValue *toBoolean(JSContext *ctx, JSValue *value) const override;

  JSValue *clone(JSContext *ctx, JSValue *value) const override;

  JSValue *pack(JSContext *ctx, JSValue *value) const override;

  JSValue *equal(JSContext *ctx, JSValue *value,
                 JSValue *another) const override;

public:
  virtual JSValue *getKeys(JSContext *ctx, JSValue *value) const;

  virtual JSValue *unpack(JSContext *ctx, JSValue *value) const;

  virtual JSValue *setField(JSContext *ctx, JSValue *object, JSValue *name,
                            JSValue *value) const;

  virtual JSValue *getField(JSContext *ctx, JSValue *object,
                            JSValue *name) const;

  virtual JSValue *defineProperty(JSContext *ctx, JSValue *object,
                                  JSValue *name, JSValue *value,
                                  bool configurable = true,
                                  bool enumable = true,
                                  bool writable = true) const;

  virtual JSValue *defineProperty(JSContext *ctx, JSValue *object,
                                  JSValue *name, JSValue *getter,
                                  JSValue *setter, bool configurable = true,
                                  bool enumable = true) const;
};