#pragma once
#include "JSObject.hpp"
#include "JSType.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSValue.hpp"
#include <string>
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
  JSField *getFieldDescriptor(JSContext *ctx, JSValue *value,
                              JSValue *name) const;

  JSField *getOwnFieldDescriptor(JSContext *ctx, JSValue *value,
                                 JSValue *name) const;

  JSValue *setPrototype(JSContext *ctx, JSValue *value,
                        JSValue *prototype) const;

  JSValue *getPrototypeOf(JSContext *ctx, JSValue *value) const;

  JSValue *setConstructor(JSContext *ctx, JSValue *value,
                          JSValue *constructor) const;

  JSValue *getConstructorOf(JSContext *ctx, JSValue *value) const;

public:
  virtual JSValue *getKeys(JSContext *ctx, JSValue *value) const;

  virtual JSValue *unpack(JSContext *ctx, JSValue *value) const;

  virtual JSValue *setField(JSContext *ctx, JSValue *object, JSValue *name,
                            JSValue *value) const;

  virtual JSValue *getField(JSContext *ctx, JSValue *object,
                            JSValue *name) const;

  JSValue *setPrivateField(JSContext *ctx, JSValue *object,
                           const std::wstring &name, JSValue *value) const;

  JSValue *getPrivateField(JSContext *ctx, JSValue *object,
                           const std::wstring &name) const;

  virtual JSValue *defineProperty(JSContext *ctx, JSValue *object,
                                  JSValue *name, JSValue *value,
                                  bool configurable = true,
                                  bool enumable = true,
                                  bool writable = true) const;

  virtual JSValue *defineProperty(JSContext *ctx, JSValue *object,
                                  JSValue *name, JSValue *getter,
                                  JSValue *setter, bool configurable = true,
                                  bool enumable = true) const;

  JSValue *definePrivateProperty(JSContext *ctx, JSValue *object,
                                 const std::wstring &name,
                                 JSValue *value) const;

  JSValue *definePrivateProperty(JSContext *ctx, JSValue *object,
                                 const std::wstring &name, JSValue *getter,
                                 JSValue *setter) const;
};