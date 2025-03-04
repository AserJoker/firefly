#include "script/engine/JSObjectType.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSException.hpp"
#include "script/engine/JSExceptionType.hpp"
#include "script/engine/JSObject.hpp"
#include "script/engine/JSString.hpp"
#include "script/engine/JSStringType.hpp"
#include "script/engine/JSType.hpp"
#include "script/engine/JSValue.hpp"
#include "script/util/JSAllocator.hpp"
#include <string>

JSObjectType::JSObjectType(JSAllocator *allocator) : JSType(allocator, 3) {}

const wchar_t *JSObjectType::getTypeName() const { return L"object"; }

JSValue *JSObjectType::toString(JSContext *ctx, JSValue *value) const {
  return ctx->createString(L"[Object object]");
}

JSValue *JSObjectType::toNumber(JSContext *ctx, JSValue *value) const {
  return ctx->createNaN();
};

JSValue *JSObjectType::toBoolean(JSContext *ctx, JSValue *value) const {
  return ctx->createBoolean(true);
};

JSValue *JSObjectType::clone(JSContext *ctx, JSValue *value) const {
  return ctx->getScope()->createValue(value->getAtom());
}

JSValue *JSObjectType::pack(JSContext *ctx, JSValue *value) const {
  return ctx->createValue(value);
};

JSValue *JSObjectType::equal(JSContext *ctx, JSValue *value,
                             JSValue *another) const {
  return ctx->createBoolean(value->getAtom() == another->getAtom());
}

JSValue *JSObjectType::getKeys(JSContext *ctx, JSValue *value) const {
  auto arr = ctx->createArray();
  ctx->pushScope();
  auto obj = value->getData()->cast<JSObject>();
  auto &fields = obj->getFields();
  uint32_t index = 0;
  for (auto &[keyAtom, field] : fields) {
    auto key = ctx->createValue(keyAtom);
    if (field.enumable && key->isTypeof<JSStringType>()) {
      auto err = ctx->setField(arr, ctx->createNumber(index), key);
      if (err) {
        return err;
      }
      index++;
    }
  }
  ctx->popScope();
  return arr;
}

JSValue *JSObjectType::unpack(JSContext *ctx, JSValue *value) const {
  return ctx->createValue(value);
}

JSValue *JSObjectType::setField(JSContext *ctx, JSValue *object, JSValue *name,
                                JSValue *value) const {
  ctx->pushScope();
  std::wstring fieldname = name->getType()
                               ->toString(ctx, name)
                               ->getData()
                               ->cast<JSString>()
                               ->getValue();
  auto obj = object->getData()->cast<JSObject>();

  JSField *pfield = nullptr;
  auto &fields = obj->getFields();
  for (auto &[keyAtom, field] : fields) {
    auto key = ctx->createValue(keyAtom);
    if (ctx->isEqual(key, name)) {
      pfield = &field;
      break;
    }
  }
  if (pfield) {
    if (pfield->value != nullptr) {
      auto oldvalue = ctx->createValue(pfield->value);
      if (oldvalue->getType() != value->getType() &&
          !ctx->isEqual(oldvalue, value)) {
        if (!pfield->writable || obj->isFrozen()) {
          return ctx->createException(
              JSException::TYPE::TYPE,
              std::format(L"Cannot assign to read only property '{}' of object "
                          L"'#<Object>'",
                          fieldname));
        } else {
          object->getAtom()->addChild(value->getAtom());
          object->getAtom()->removeChild(oldvalue->getAtom());
          pfield->value = value->getAtom();
        }
      }
    } else {
      if (!pfield->setter) {
        return ctx->createException(
            JSException::TYPE::TYPE,
            std::format(L"Cannot assign to read only property '{}' of object "
                        L"'#<Object>'",
                        fieldname));
      }
      auto setter = ctx->createValue(pfield->setter);
      auto err = ctx->call(setter, object, {name, value});
      if (err->isTypeof<JSExceptionType>()) {
        return err;
      }
    }
  } else {
    auto err = defineProperty(ctx, object, name, value);
    if (err) {
      return err;
    }
  }
  ctx->popScope();
  return nullptr;
}

JSValue *JSObjectType::getField(JSContext *ctx, JSValue *object,
                                JSValue *name) const {
  auto current = ctx->getScope();
  JSValue *result = nullptr;
  ctx->pushScope();
  JSAtom *resAtom = nullptr;
  auto &fields = object->getData()->cast<JSObject>()->getFields();
  for (auto &[keyAtom, field] : fields) {
    auto key = ctx->createValue(keyAtom);
    if (ctx->isEqual(key, name)) {
      if (field.value) {
        resAtom = field.value;
      } else {
        auto getter = ctx->createValue(field.getter);
        auto res = ctx->call(getter, object, {name});
        if (res->isTypeof<JSExceptionType>()) {
          return res;
        }
        resAtom = res->getAtom();
      }
    }
  }
  if (resAtom) {
    result = current->createValue(resAtom);
  }
  ctx->popScope();
  if (result) {
    return result;
  } else {
    return ctx->createUndefined();
  }
}

JSValue *JSObjectType::defineProperty(JSContext *ctx, JSValue *object,
                                      JSValue *name, JSValue *value,
                                      bool configurable, bool enumable,
                                      bool writable) const {

  auto obj = object->getData()->cast<JSObject>();
  ctx->pushScope();
  std::wstring fieldname = name->getType()
                               ->toString(ctx, name)
                               ->getData()
                               ->cast<JSString>()
                               ->getValue();
  auto &fields = obj->getFields();
  JSField *field = nullptr;
  for (auto &[keyAtom, f] : fields) {
    auto key = ctx->createValue(keyAtom);
    if (ctx->isEqual(key, name)) {
      field = &f;
      break;
    }
  }
  if (field) {
    if (!field->configurable) {
      if (field->configurable != configurable || field->enumable != enumable ||
          field->getter != nullptr) {
        return ctx->createException(
            JSException::TYPE::TYPE,
            std::format(L"Cannot assign to read only property '{}' of object "
                        L"'#<Object>'",
                        fieldname));
      }
    }
    auto oldvalue = ctx->createValue(field->value);
    if (oldvalue->getType() != value->getType() ||
        !ctx->isEqual(oldvalue, value)) {
      if (!field->writable || obj->isFrozen()) {
        return ctx->createException(
            JSException::TYPE::TYPE,
            std::format(L"Cannot assign to read only property '{}' of object "
                        L"'#<Object>'",
                        fieldname));
      }
      field->configurable = configurable;
      field->enumable = enumable;
      field->writable = writable;
      object->getAtom()->addChild(value->getAtom());
      object->getAtom()->removeChild(field->value);
      field->value = value->getAtom();
    }
  } else {
    if (obj->isSealed() || !obj->isExtensible() || obj->isFrozen()) {
      return ctx->createException(
          JSException::TYPE::TYPE,
          std::format(L"Cannot assign to read only property '{}' of object "
                      L"'#<Object>'",
                      fieldname));
    }
    object->getAtom()->addChild(value->getAtom());
    object->getAtom()->addChild(name->getAtom());
    fields[name->getAtom()] = {
        .configurable = configurable,
        .enumable = enumable,
        .value = value->getAtom(),
        .writable = writable,
        .getter = nullptr,
        .setter = nullptr,
    };
  }
  ctx->popScope();
  return nullptr;
}

JSValue *JSObjectType::defineProperty(JSContext *ctx, JSValue *object,
                                      JSValue *name, JSValue *getter,
                                      JSValue *setter, bool configurable,
                                      bool enumable) const {
  auto obj = object->getData()->cast<JSObject>();
  ctx->pushScope();
  std::wstring fieldname = name->getType()
                               ->toString(ctx, name)
                               ->getData()
                               ->cast<JSString>()
                               ->getValue();
  auto &fields = obj->getFields();
  JSField *field = nullptr;
  for (auto &[keyAtom, f] : fields) {
    auto key = ctx->createValue(keyAtom);
    if (ctx->isEqual(key, name)) {
      field = &f;
      break;
    }
  }
  if (field) {
    if (!field->configurable) {
      if (field->configurable != configurable || field->enumable != enumable ||
          field->value != nullptr) {
        return ctx->createException(
            JSException::TYPE::TYPE,
            std::format(L"Cannot assign to read only property '{}' of object "
                        L"'#<Object>'",
                        fieldname));
      }
    }
    field->configurable = configurable;
    field->enumable = enumable;
    object->getAtom()->addChild(getter->getAtom());
    if (setter) {
      object->getAtom()->addChild(setter->getAtom());
    }
    object->getAtom()->removeChild(field->getter);
    if (field->setter) {
      object->getAtom()->removeChild(field->setter);
    }
    field->getter = getter->getAtom();
    field->setter = setter == nullptr ? nullptr : setter->getAtom();
  } else {
    if (obj->isSealed() || !obj->isExtensible() || obj->isFrozen()) {
      return ctx->createException(
          JSException::TYPE::TYPE,
          std::format(L"Cannot assign to read only property '{}' of object "
                      L"'#<Object>'",
                      fieldname));
    }
    object->getAtom()->addChild(getter->getAtom());
    object->getAtom()->addChild(name->getAtom());
    if (setter) {
      object->getAtom()->addChild(setter->getAtom());
    }
    fields[name->getAtom()] = {
        .configurable = configurable,
        .enumable = enumable,
        .value = nullptr,
        .writable = true,
        .getter = getter->getAtom(),
        .setter = setter == nullptr ? nullptr : setter->getAtom(),
    };
  }
  ctx->popScope();
  return nullptr;
}