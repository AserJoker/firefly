#pragma once
#include "JSType.hpp"
class JSNumberType : public JSType {
public:
  JSNumberType(JSAllocator *allocator);

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
  virtual JSValue *add(JSContext *ctx, JSValue *value, JSValue *another) const;

  virtual JSValue *sub(JSContext *ctx, JSValue *value, JSValue *another) const;

  virtual JSValue *mul(JSContext *ctx, JSValue *value, JSValue *another) const;

  virtual JSValue *div(JSContext *ctx, JSValue *value, JSValue *another) const;

  virtual JSValue *mod(JSContext *ctx, JSValue *value, JSValue *another) const;

  virtual JSValue *pow(JSContext *ctx, JSValue *value, JSValue *another) const;

  virtual JSValue *and_(JSContext *ctx, JSValue *value, JSValue *another) const;

  virtual JSValue *or_(JSContext *ctx, JSValue *value, JSValue *another) const;

  virtual JSValue *xor_(JSContext *ctx, JSValue *value, JSValue *another) const;

  virtual JSValue *shr(JSContext *ctx, JSValue *value, JSValue *another) const;

  virtual JSValue *shl(JSContext *ctx, JSValue *value, JSValue *another) const;

  virtual JSValue *gt(JSContext *ctx, JSValue *value, JSValue *another) const;

  virtual JSValue *ge(JSContext *ctx, JSValue *value, JSValue *another) const;

  virtual JSValue *lt(JSContext *ctx, JSValue *value, JSValue *another) const;

  virtual JSValue *le(JSContext *ctx, JSValue *value, JSValue *another) const;

  virtual JSValue *not_(JSContext *ctx, JSValue *value) const;

  virtual JSValue *inc(JSContext *ctx, JSValue *value) const;

  virtual JSValue *dec(JSContext *ctx, JSValue *value) const;

  virtual JSValue *unaryNegation(JSContext *ctx, JSValue *value) const;

  virtual JSValue *unaryPlus(JSContext *ctx, JSValue *value) const;
};