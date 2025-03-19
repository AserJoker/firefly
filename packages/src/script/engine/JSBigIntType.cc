#include "script/engine/JSBigIntType.hpp"
#include "script/engine/JSBigInt.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSException.hpp"
#include "script/engine/JSValue.hpp"
#include "script/util/BigInt.hpp"
#include "script/util/JSAllocator.hpp"
#include <cmath>

JSBigIntType::JSBigIntType(JSAllocator *allocator) : JSType(allocator) {}

const wchar_t *JSBigIntType::getTypeName() const { return L"bigint"; }

JSValue *JSBigIntType::toString(JSContext *ctx, JSValue *value) const {
  auto number = value->getData()->cast<JSBigInt>();
  return ctx->createString(number->getValue().toString() + L'n');
}

JSValue *JSBigIntType::toNumber(JSContext *ctx, JSValue *value) const {
  auto bigint = value->getData()->cast<JSBigInt>();
  auto i64 = bigint->getValue().toInt64();
  if (i64.has_value()) {
    if (i64.value() > std::pow(2, 53) - 1) {
      return ctx->createInfinity();
    } else if (i64.value() < -(std::pow(2, 53) - 1)) {
      return ctx->createInfinity(true);
    }
    return ctx->createNumber(i64.value());
  } else {
    return ctx->createInfinity(bigint->getValue().isNetative());
  }
}

JSValue *JSBigIntType::toBoolean(JSContext *ctx, JSValue *value) const {
  return ctx->createBoolean(value->getData()->cast<JSBigInt>()->getValue() !=
                            0);
}

JSValue *JSBigIntType::clone(JSContext *ctx, JSValue *value) const {
  return ctx->createBigInt(value->getData()->cast<JSBigInt>()->getValue());
}

JSValue *JSBigIntType::pack(JSContext *ctx, JSValue *value) const {
  auto BigInt = ctx->getBigIntConstructor();
  return ctx->construct(BigInt, {value});
};

JSValue *JSBigIntType::equal(JSContext *ctx, JSValue *value,
                             JSValue *another) const {
  return ctx->createBoolean(value->getData()->cast<JSBigInt>()->getValue() ==
                            another->getData()->cast<JSBigInt>()->getValue());
}

JSValue *JSBigIntType::add(JSContext *ctx, JSValue *value,
                           JSValue *another) const {
  return ctx->createBigInt(value->getData()->cast<JSBigInt>()->getValue() +
                           another->getData()->cast<JSBigInt>()->getValue());
}

JSValue *JSBigIntType::sub(JSContext *ctx, JSValue *value,
                           JSValue *another) const {
  return ctx->createBigInt(value->getData()->cast<JSBigInt>()->getValue() -
                           another->getData()->cast<JSBigInt>()->getValue());
}

JSValue *JSBigIntType::mul(JSContext *ctx, JSValue *value,
                           JSValue *another) const {
  return ctx->createBigInt(value->getData()->cast<JSBigInt>()->getValue() *
                           another->getData()->cast<JSBigInt>()->getValue());
}

JSValue *JSBigIntType::div(JSContext *ctx, JSValue *value,
                           JSValue *another) const {
  return ctx->createBigInt(value->getData()->cast<JSBigInt>()->getValue() /
                           another->getData()->cast<JSBigInt>()->getValue());
}

JSValue *JSBigIntType::mod(JSContext *ctx, JSValue *value,
                           JSValue *another) const {
  return ctx->createBigInt(value->getData()->cast<JSBigInt>()->getValue() %
                           another->getData()->cast<JSBigInt>()->getValue());
}

JSValue *JSBigIntType::unaryNegation(JSContext *ctx, JSValue *value) const {
  return ctx->createBigInt(-value->getData()->cast<JSBigInt>()->getValue());
};

JSValue *JSBigIntType::pow(JSContext *ctx, JSValue *value,
                           JSValue *another) const {
  return ctx->createBigInt(value->getData()->cast<JSBigInt>()->getValue().pow(
      another->getData()->cast<JSBigInt>()->getValue()));
}

JSValue *JSBigIntType::and_(JSContext *ctx, JSValue *value,
                            JSValue *another) const {
  return ctx->createBigInt(value->getData()->cast<JSBigInt>()->getValue() &
                           another->getData()->cast<JSBigInt>()->getValue());
}

JSValue *JSBigIntType::or_(JSContext *ctx, JSValue *value,
                           JSValue *another) const {
  return ctx->createBigInt(value->getData()->cast<JSBigInt>()->getValue() |
                           another->getData()->cast<JSBigInt>()->getValue());
}

JSValue *JSBigIntType::not_(JSContext *ctx, JSValue *value) const {
  return ctx->createBigInt(~value->getData()->cast<JSBigInt>()->getValue());
}

JSValue *JSBigIntType::inc(JSContext *ctx, JSValue *value) const {
  if (value->isConst()) {
    return ctx->createException(JSException::TYPE::TYPE,
                                L"Assignment to constant variable");
  }
  auto val = value->getData()->cast<JSBigInt>()->getValue();
  val += 1;
  value->getData()->cast<JSBigInt>()->setValue(val);
  return value;
};

JSValue *JSBigIntType::dec(JSContext *ctx, JSValue *value) const {
  if (value->isConst()) {
    return ctx->createException(JSException::TYPE::TYPE,
                                L"Assignment to constant variable");
  }
  auto val = value->getData()->cast<JSBigInt>()->getValue();
  val -= 1;
  value->getData()->cast<JSBigInt>()->setValue(val);
  return value;
}

JSValue *JSBigIntType::xor_(JSContext *ctx, JSValue *value,
                            JSValue *another) const {
  return ctx->createBigInt(value->getData()->cast<JSBigInt>()->getValue() ^
                           another->getData()->cast<JSBigInt>()->getValue());
}

JSValue *JSBigIntType::shr(JSContext *ctx, JSValue *value,
                           JSValue *another) const {
  return ctx->createBigInt(value->getData()->cast<JSBigInt>()->getValue() >>
                           another->getData()->cast<JSBigInt>()->getValue());
}

JSValue *JSBigIntType::shl(JSContext *ctx, JSValue *value,
                           JSValue *another) const {
  return ctx->createBigInt(value->getData()->cast<JSBigInt>()->getValue()
                           << another->getData()->cast<JSBigInt>()->getValue());
}

JSValue *JSBigIntType::gt(JSContext *ctx, JSValue *value,
                          JSValue *another) const {
  return ctx->createBigInt(value->getData()->cast<JSBigInt>()->getValue() >
                           another->getData()->cast<JSBigInt>()->getValue());
}

JSValue *JSBigIntType::ge(JSContext *ctx, JSValue *value,
                          JSValue *another) const {
  return ctx->createBigInt(value->getData()->cast<JSBigInt>()->getValue() >=
                           another->getData()->cast<JSBigInt>()->getValue());
}

JSValue *JSBigIntType::lt(JSContext *ctx, JSValue *value,
                          JSValue *another) const {
  return ctx->createBigInt(value->getData()->cast<JSBigInt>()->getValue() <
                           another->getData()->cast<JSBigInt>()->getValue());
}

JSValue *JSBigIntType::le(JSContext *ctx, JSValue *value,
                          JSValue *another) const {
  return ctx->createBigInt(value->getData()->cast<JSBigInt>()->getValue() <=
                           another->getData()->cast<JSBigInt>()->getValue());
}