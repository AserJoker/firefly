#include "script/engine/JSNumberType.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSInfinity.hpp"
#include "script/engine/JSInfinityType.hpp"
#include "script/engine/JSNaNType.hpp"
#include "script/engine/JSNumber.hpp"
#include "script/engine/JSValue.hpp"
#include "script/util/JSAllocator.hpp"
#include <cstdint>
JSNumberType::JSNumberType(JSAllocator *allocator) : JSType(allocator) {}

const wchar_t *JSNumberType::getTypeName() const { return L"number"; }

JSValue *JSNumberType::toString(JSContext *ctx, JSValue *value) const {
  auto number = value->getData()->cast<JSNumber>();
  return ctx->createString(std::format(L"{}", number->getValue()));
}

JSValue *JSNumberType::toNumber(JSContext *ctx, JSValue *value) const {
  return value;
}

JSValue *JSNumberType::toBoolean(JSContext *ctx, JSValue *value) const {
  return ctx->createBoolean(value->getData()->cast<JSNumber>()->getValue() !=
                            0);
}

JSValue *JSNumberType::clone(JSContext *ctx, JSValue *value) const {
  return ctx->createNumber(value->getData()->cast<JSNumber>()->getValue());
}

JSValue *JSNumberType::pack(JSContext *ctx, JSValue *value) const {
  auto Number = ctx->getNumberConstructor();
  return ctx->construct(Number, {value});
};

JSValue *JSNumberType::equal(JSContext *ctx, JSValue *value,
                             JSValue *another) const {
  if (value->isTypeof<JSNaNType>() || another->isTypeof<JSNaNType>()) {
    return ctx->createBoolean(false);
  }
  if (value->isTypeof<JSInfinityType>() ||
      another->isTypeof<JSInfinityType>()) {
    auto left = value->getData()->cast<JSInfinity>();
    auto right = value->getData()->cast<JSInfinity>();
    return ctx->createBoolean(left && right &&
                              left->isNegative() == right->isNegative());
  }
  return ctx->createBoolean(ctx->checkedNumber(ctx->toNumber(value)) ==
                            ctx->checkedNumber(ctx->toNumber(another)));
}
JSValue *JSNumberType::add(JSContext *ctx, JSValue *value,
                           JSValue *another) const {
  if (another->isTypeof<JSNaNType>() || value->isTypeof<JSNaNType>()) {
    return ctx->createNaN();
  }
  if (another->isTypeof<JSInfinityType>()) {
    auto tmp = another;
    another = value;
    value = tmp;
  }
  if (value->isTypeof<JSInfinityType>()) {
    if (!another->isTypeof<JSInfinityType>()) {
      return ctx->createInfinity(
          value->getData()->cast<JSInfinity>()->isNegative());
    } else {
      if (value->getData()->cast<JSInfinity>()->isNegative() !=
          another->getData()->cast<JSInfinity>()->isNegative()) {
        return ctx->createNumber(0);
      } else {
        return ctx->createInfinity(
            value->getData()->cast<JSInfinity>()->isNegative());
      }
    }
  }
  return ctx->createNumber(ctx->checkedNumber(value) +
                           ctx->checkedNumber(another));
}

JSValue *JSNumberType::sub(JSContext *ctx, JSValue *value,
                           JSValue *another) const {
  if (another->isTypeof<JSNaNType>() || value->isTypeof<JSNaNType>()) {
    return ctx->createNaN();
  }
  if (value->isTypeof<JSInfinityType>()) {
    if (!another->isTypeof<JSInfinityType>()) {
      return ctx->createInfinity(
          value->getData()->cast<JSInfinity>()->isNegative());
    } else {
      if (value->getData()->cast<JSInfinity>()->isNegative() ==
          another->getData()->cast<JSInfinity>()->isNegative()) {
        return ctx->createNumber(0);
      } else {
        return ctx->createInfinity(
            value->getData()->cast<JSInfinity>()->isNegative());
      }
    }
  }
  if (another->isTypeof<JSInfinityType>()) {
    return ctx->createInfinity(
        !another->getData()->cast<JSInfinity>()->isNegative());
  }
  return ctx->createNumber(ctx->checkedNumber(value) -
                           ctx->checkedNumber(another));
}

JSValue *JSNumberType::mul(JSContext *ctx, JSValue *value,
                           JSValue *another) const {
  if (another->isTypeof<JSNaNType>() || value->isTypeof<JSNaNType>()) {
    return ctx->createNaN();
  }
  if (another->isTypeof<JSInfinityType>()) {
    auto tmp = another;
    another = value;
    value = tmp;
  }
  if (value->isTypeof<JSInfinityType>()) {
    if (another->isTypeof<JSInfinityType>()) {
      return ctx->createInfinity(
          value->getData()->cast<JSInfinity>()->isNegative() &&
          another->getData()->cast<JSInfinity>()->isNegative());
    }
    auto val = another->getData()->cast<JSNumber>()->getValue();
    if (val == 0) {
      return ctx->createNaN();
    }
    return ctx->createInfinity(
        (value->getData()->cast<JSInfinity>()->isNegative()) == (val < 0));
  }
  return ctx->createNumber(ctx->checkedNumber(value) *
                           ctx->checkedNumber(another));
}

JSValue *JSNumberType::div(JSContext *ctx, JSValue *value,
                           JSValue *another) const {
  if (another->isTypeof<JSNaNType>() || value->isTypeof<JSNaNType>()) {
    return ctx->createNaN();
  }
  if (value->isTypeof<JSInfinityType>()) {
    if (another->isTypeof<JSInfinityType>()) {
      return ctx->createNaN();
    }
  }
  if (another->isTypeof<JSInfinityType>()) {
    return ctx->createNumber(0);
  }
  if (another->getData()->cast<JSNumber>()->getValue() == 0) {
    return ctx->createInfinity(value->getData()->cast<JSNumber>()->getValue() <
                               0);
  }
  return ctx->createNumber(ctx->checkedNumber(value) /
                           ctx->checkedNumber(another));
}

JSValue *JSNumberType::mod(JSContext *ctx, JSValue *value,
                           JSValue *another) const {

  if (another->isTypeof<JSNaNType>() || value->isTypeof<JSNaNType>()) {
    return ctx->createNaN();
  }
  if (value->isTypeof<JSInfinityType>()) {
    return ctx->createNaN();
  }
  if (another->isTypeof<JSInfinityType>()) {
    return value;
  }
  return ctx->createNumber((int64_t)ctx->checkedNumber(value) %
                           (int64_t)ctx->checkedNumber(another));
}

JSValue *JSNumberType::pow(JSContext *ctx, JSValue *value,
                           JSValue *another) const {
  if (another->isTypeof<JSNaNType>() || value->isTypeof<JSNaNType>()) {
    return ctx->createNaN();
  }
  if (value->isTypeof<JSInfinityType>()) {
    if (another->isTypeof<JSInfinityType>()) {
      return ctx->createNaN();
    }
    auto val = another->getData()->cast<JSNumber>()->getValue();
    if (val == 0) {
      return ctx->createNumber(1);
    } else if (val < 0) {
      return ctx->createNumber(0);
    } else if (val == (int64_t)val) {
      return ctx->createInfinity(
          ((int64_t)val % 2 == 0)
              ? false
              : value->getData()->cast<JSInfinity>()->isNegative());
    } else {
      return ctx->createInfinity();
    }
  }
  if (another->isTypeof<JSInfinityType>()) {
    if (value->getData()->cast<JSNumber>()->getValue() == 0) {
      return ctx->createNumber(0);
    } else {
      return ctx->createNaN();
    }
  }

  return ctx->createNumber(
      std::pow(ctx->checkedNumber(value), ctx->checkedNumber(another)));
}

JSValue *JSNumberType::and_(JSContext *ctx, JSValue *value,
                            JSValue *another) const {
  if (value->isTypeof<JSNaNType>() || value->isTypeof<JSInfinityType>()) {
    return ctx->createNumber(0);
  }
  if (another->isTypeof<JSNaNType>() || another->isTypeof<JSInfinityType>()) {
    return ctx->createNumber(0);
  }

  return ctx->createNumber((int64_t)ctx->checkedNumber(value) &
                           (int64_t)ctx->checkedNumber(another));
}

JSValue *JSNumberType::or_(JSContext *ctx, JSValue *value,
                           JSValue *another) const {
  if (value->isTypeof<JSNaNType>() || value->isTypeof<JSInfinityType>()) {
    if (another->isTypeof<JSNaNType>() || another->isTypeof<JSInfinityType>()) {
      return ctx->createNumber(0);
    } else {
      return another;
    }
  }
  if (another->isTypeof<JSNaNType>() || another->isTypeof<JSInfinityType>()) {
    return value;
  }

  return ctx->createNumber((int64_t)ctx->checkedNumber(value) |
                           (int64_t)ctx->checkedNumber(another));
}

JSValue *JSNumberType::xor_(JSContext *ctx, JSValue *value,
                            JSValue *another) const {
  if (value->isTypeof<JSNaNType>() || value->isTypeof<JSInfinityType>()) {
    if (another->isTypeof<JSNaNType>() || another->isTypeof<JSInfinityType>()) {
      return ctx->createNumber(0);
    } else {
      return another;
    }
  }
  if (another->isTypeof<JSNaNType>() || another->isTypeof<JSInfinityType>()) {
    return value;
  }
  return ctx->createNumber((int64_t)ctx->checkedNumber(value) ^
                           (int64_t)ctx->checkedNumber(another));
}

JSValue *JSNumberType::shr(JSContext *ctx, JSValue *value,
                           JSValue *another) const {
  if (value->isTypeof<JSNaNType>() || value->isTypeof<JSInfinityType>()) {
    return ctx->createNumber(0);
  }
  if (another->isTypeof<JSNaNType>() || another->isTypeof<JSInfinityType>()) {
    return value;
  }
  return ctx->createNumber((int64_t)ctx->checkedNumber(value) >>
                           (int64_t)ctx->checkedNumber(another));
}

JSValue *JSNumberType::shl(JSContext *ctx, JSValue *value,
                           JSValue *another) const {
  if (value->isTypeof<JSNaNType>() || value->isTypeof<JSInfinityType>()) {
    return ctx->createNumber(0);
  }
  if (another->isTypeof<JSNaNType>() || another->isTypeof<JSInfinityType>()) {
    return value;
  }
  return ctx->createNumber((int64_t)ctx->checkedNumber(value)
                           << (int64_t)ctx->checkedNumber(another));
}

JSValue *JSNumberType::gt(JSContext *ctx, JSValue *value,
                          JSValue *another) const {
  if (value->isTypeof<JSNaNType>() || value->isTypeof<JSInfinityType>()) {
    return ctx->createBoolean(false);
  }
  if (value->isTypeof<JSInfinityType>()) {
    auto inf = value->getData()->cast<JSInfinity>();
    if (another->isTypeof<JSInfinityType>()) {
      auto ano = another->getData()->cast<JSInfinity>();
      if (!inf->isNegative()) {
        if (ano->isNegative()) {
          return ctx->createBoolean(true);
        }
      }
      return ctx->createBoolean(false);
    }
  }
  if (another->isTypeof<JSInfinityType>()) {
    auto ano = another->getData()->cast<JSInfinity>();
    if (ano->isNegative()) {
      return ctx->createBoolean(true);
    }
    return ctx->createBoolean(false);
  }
  return ctx->createNumber(ctx->checkedNumber(value) >
                           ctx->checkedNumber(another));
}

JSValue *JSNumberType::ge(JSContext *ctx, JSValue *value,
                          JSValue *another) const {
  if (value->isTypeof<JSNaNType>() || value->isTypeof<JSInfinityType>()) {
    return ctx->createBoolean(false);
  }
  if (value->isTypeof<JSInfinityType>()) {
    auto inf = value->getData()->cast<JSInfinity>();
    if (another->isTypeof<JSInfinityType>()) {
      auto ano = another->getData()->cast<JSInfinity>();
      if (!inf->isNegative() || inf->isNegative() == ano->isNegative()) {
        return ctx->createBoolean(true);
      }
      return ctx->createBoolean(false);
    }
  }
  if (another->isTypeof<JSInfinityType>()) {
    auto ano = another->getData()->cast<JSInfinity>();
    if (ano->isNegative()) {
      return ctx->createBoolean(true);
    }
    return ctx->createBoolean(false);
  }
  return ctx->createNumber(ctx->checkedNumber(value) >=
                           ctx->checkedNumber(another));
}

JSValue *JSNumberType::lt(JSContext *ctx, JSValue *value,
                          JSValue *another) const {
  if (value->isTypeof<JSNaNType>() || value->isTypeof<JSInfinityType>()) {
    return ctx->createBoolean(false);
  }
  if (value->isTypeof<JSInfinityType>()) {
    auto inf = value->getData()->cast<JSInfinity>();
    if (another->isTypeof<JSInfinityType>()) {
      auto ano = another->getData()->cast<JSInfinity>();
      if (inf->isNegative()) {
        if (!ano->isNegative()) {
          return ctx->createBoolean(true);
        }
      }
      return ctx->createBoolean(false);
    }
  }
  if (another->isTypeof<JSInfinityType>()) {
    auto ano = another->getData()->cast<JSInfinity>();
    if (!ano->isNegative()) {
      return ctx->createBoolean(true);
    }
    return ctx->createBoolean(false);
  }
  return ctx->createNumber(ctx->checkedNumber(value) <
                           ctx->checkedNumber(another));
}

JSValue *JSNumberType::le(JSContext *ctx, JSValue *value,
                          JSValue *another) const {
  if (value->isTypeof<JSNaNType>() || value->isTypeof<JSInfinityType>()) {
    return ctx->createBoolean(false);
  }
  if (value->isTypeof<JSInfinityType>()) {
    auto inf = value->getData()->cast<JSInfinity>();
    if (another->isTypeof<JSInfinityType>()) {
      auto ano = another->getData()->cast<JSInfinity>();
      if (inf->isNegative() || inf->isNegative() == ano->isNegative()) {
        return ctx->createBoolean(true);
      }
      return ctx->createBoolean(false);
    }
  }
  if (another->isTypeof<JSInfinityType>()) {
    auto ano = another->getData()->cast<JSInfinity>();
    if (!ano->isNegative()) {
      return ctx->createBoolean(true);
    }
    return ctx->createBoolean(false);
  }
  return ctx->createNumber(ctx->checkedNumber(value) <=
                           ctx->checkedNumber(another));
}

JSValue *JSNumberType::not_(JSContext *ctx, JSValue *value) const {
  if (value->isTypeof<JSNaNType>() || value->isTypeof<JSInfinityType>()) {
    return ctx->createNumber(-1);
  }
  return ctx->createNumber(~(int64_t)ctx->checkedNumber(value));
}
JSValue *JSNumberType::inc(JSContext *ctx, JSValue *value) const {
  if (value->isTypeof<JSNaNType>() || value->isTypeof<JSInfinityType>()) {
    return value;
  }
  value->getData()->cast<JSNumber>()->setValue(ctx->checkedNumber(value) + 1);
  return value;
}

JSValue *JSNumberType::dec(JSContext *ctx, JSValue *value) const {
  if (value->isTypeof<JSNaNType>() || value->isTypeof<JSInfinityType>()) {
    return value;
  }
  value->getData()->cast<JSNumber>()->setValue(ctx->checkedNumber(value) - 1);
  return value;
}

JSValue *JSNumberType::unaryNegation(JSContext *ctx, JSValue *value) const {
  return ctx->createNumber(-value->getData()->cast<JSNumber>()->getValue());
}

JSValue *JSNumberType::unaryPlus(JSContext *ctx, JSValue *value) const {
  return value;
}