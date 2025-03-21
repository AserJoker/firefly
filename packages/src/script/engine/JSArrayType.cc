#include "script/engine/JSArrayType.hpp"
#include "script/engine/JSArray.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSException.hpp"
#include "script/engine/JSInfinityType.hpp"
#include "script/engine/JSNaNType.hpp"
#include "script/engine/JSNumberType.hpp"
#include "script/engine/JSObjectType.hpp"
#include "script/engine/JSValue.hpp"
#include "script/util/JSAllocator.hpp"
#include <sstream>

JSArrayType ::JSArrayType(JSAllocator *allocator) : JSObjectType(allocator) {}

JSValue *JSArrayType::toString(JSContext *ctx, JSValue *value) const {
  std::wstringstream ss;
  auto arr = value->getData()->cast<JSArray>();
  auto &items = arr->getItems();
  ctx->pushScope();
  for (size_t index = 0; index < arr->getLength(); index++) {
    if (items.contains(index)) {
      auto str = ctx->toString(ctx->getScope()->createValue(items[index]));
      CHECK(ctx, str);
      ss << ctx->checkedString(str);
    }
    if (index != arr->getLength() - 1) {
      ss << L",";
    }
  }
  ctx->popScope();
  return ctx->createString(ss.str());
}

JSValue *JSArrayType::getField(JSContext *ctx, JSValue *array,
                               JSValue *name) const {
  auto numval = ctx->toNumber(name);
  if (numval->isTypeof<JSNumberType>() && !numval->isTypeof<JSNaNType>() &&
      !numval->isTypeof<JSInfinityType>()) {
    auto num = ctx->checkedNumber(numval);
    size_t idx = (size_t)num;
    if (idx == num) {
      auto arr = array->getData()->cast<JSArray>();
      auto &items = arr->getItems();
      if (items.contains(idx)) {
        return ctx->createValue(arr->getItems().at(idx));
      }
      return ctx->createUndefined();
    }
  }
  if (ctx->checkedString(name) == L"length") {
    return ctx->createNumber(array->getData()->cast<JSArray>()->getLength());
  }
  return JSObjectType::getField(ctx, array, name);
};

JSValue *JSArrayType::setField(JSContext *ctx, JSValue *array, JSValue *name,
                               JSValue *value) const {
  auto numval = ctx->toNumber(name);
  if (numval->isTypeof<JSNumberType>() && !numval->isTypeof<JSNaNType>() &&
      !numval->isTypeof<JSInfinityType>()) {
    auto num = ctx->checkedNumber(numval);
    size_t idx = (size_t)num;
    if (idx == num) {
      auto arr = array->getData()->cast<JSArray>();
      auto &items = arr->getItems();
      if (items.contains(idx)) {
        auto oldval = ctx->createValue(items.at(idx));
        if (oldval->getType() != value->getType() ||
            !ctx->isEqual(oldval, value)) {
          if (arr->isFrozen()) {
            return ctx->createException(
                JSException::TYPE::TYPE,
                std::format(
                    L"Cannot assign to read only property '{}' of object "
                    L"'#<Object>'",
                    idx));
          }
          array->getAtom()->removeChild(oldval->getAtom());
          ctx->recycle(oldval->getAtom());
          array->getAtom()->addChild(value->getAtom());
          items[idx] = value->getAtom();
        }
      } else {
        if (arr->isFrozen() || !arr->isExtensible() || arr->isSealed()) {
          return ctx->createException(
              JSException::TYPE::TYPE,
              std::format(L"Cannot add property {}, object is not extensible",
                          idx));
        }
        array->getAtom()->addChild(value->getAtom());
        items[idx] = value->getAtom();
        if (idx >= arr->getLength()) {
          arr->setLength(idx + 1);
          auto err = ctx->defineProperty(array, ctx->createString(L"array"),
                                         ctx->createNumber(arr->getLength()),
                                         true, false, true);
          CHECK(ctx, err);
        }
      }
      return ctx->createUndefined();
    }
  }
  if (ctx->checkedString(name) == L"length") {

    auto arr = array->getData()->cast<JSArray>();
    auto &items = arr->getItems();
    value = ctx->toNumber(value);
    if (value->isTypeof<JSNaNType>() || value->isTypeof<JSInfinityType>()) {
      return ctx->createException(JSException::TYPE::RANGE,
                                  L"Invalid array length");
    }
    if (arr->isSealed()) {
      return ctx->createUndefined();
    }
    if (arr->isFrozen() || !arr->isExtensible()) {
      return ctx->createException(
          JSException::TYPE::TYPE,
          std::format(L"Cannot assign to read only property 'length' of object "
                      L"'[object Array]'"));
    }
    auto len = ctx->checkedNumber(value);
    for (auto it = items.begin(); it != items.end(); it++) {
      if (it->first >= len) {
        ctx->recycle(it->second);
        array->getAtom()->removeChild(it->second);
        items.erase(it);
        it = items.begin();
        continue;
      }
    }
    arr->setLength(len);
    return ctx->createUndefined();
  }
  return JSObjectType::setField(ctx, array, name, value);
};