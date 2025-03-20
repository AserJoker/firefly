#include "script/engine/JSObjectType.hpp"
#include "script/engine/JSCallable.hpp"
#include "script/engine/JSCallableType.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSException.hpp"
#include "script/engine/JSExceptionType.hpp"
#include "script/engine/JSObject.hpp"
#include "script/engine/JSStringType.hpp"
#include "script/engine/JSSymbolType.hpp"
#include "script/engine/JSType.hpp"
#include "script/engine/JSUndefinedType.hpp"
#include "script/engine/JSValue.hpp"
#include "script/util/JSAllocator.hpp"
#include <string>

JSObjectType::JSObjectType(JSAllocator *allocator) : JSType(allocator) {}

const wchar_t *JSObjectType::getTypeName() const { return L"object"; }

JSValue *JSObjectType::toString(JSContext *ctx, JSValue *value) const {
  auto toString = ctx->getField(value, ctx->createString(L"toString"));
  CHECK(ctx, toString);
  if (toString->isTypeof<JSCallableType>()) {
    return ctx->call(toString, value, {});
  } else if (!toString->isTypeof<JSUndefinedType>()) {
    return ctx->createException(JSException::TYPE::TYPE,
                                L"Cannot convert object to primitive value");
  }
  auto Object = ctx->getObjectConstructor();
  CHECK(ctx, Object);
  if (!Object->isTypeof<JSCallableType>()) {
    return ctx->createException(JSException::TYPE::TYPE,
                                L"'Object' is not function: ");
  }
  auto prototype = ctx->getField(Object, ctx->createString(L"prototype"));
  CHECK(ctx, prototype);
  if (!prototype->isTypeof<JSObjectType>()) {
    return ctx->createException(JSException::TYPE::TYPE,
                                L"'Object.prototype' is not object");
  }
  toString = ctx->getField(prototype, ctx->createString(L"toString"));
  CHECK(ctx, toString);
  if (!toString->isTypeof<JSCallableType>()) {
    return ctx->createException(JSException::TYPE::TYPE,
                                L"Cannot convert object to primitive value");
  }
  return ctx->call(toString, value, {});
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
  return ctx->createBoolean(value->getData() == another->getData());
}

JSField *JSObjectType::getFieldDescriptor(JSContext *ctx, JSValue *value,
                                          JSValue *name) const {
  JSField *field = nullptr;
  auto object = value->getData()->cast<JSObject>();
  ctx->pushScope();
  while (!field) {
    if (!object) {
      break;
    }
    auto &fields = object->getFields();
    for (auto &[keyAtom, f] : fields) {
      auto key = ctx->createValue(keyAtom);
      if (ctx->checkedBoolean(ctx->isEqual(key, name))) {
        field = &f;
        break;
      }
    }
    object = object->getPrototype()->getData()->cast<JSObject>();
  }
  ctx->popScope();
  return field;
}

JSField *JSObjectType::getOwnFieldDescriptor(JSContext *ctx, JSValue *value,
                                             JSValue *name) const {
  auto object = value->getData()->cast<JSObject>();
  ctx->pushScope();
  auto &fields = object->getFields();
  for (auto &[keyAtom, field] : fields) {
    auto key = ctx->createValue(keyAtom);
    if (ctx->checkedBoolean(ctx->isEqual(key, name))) {
      ctx->popScope();
      return &field;
    }
  }
  ctx->popScope();
  return nullptr;
}

JSValue *JSObjectType::setPrototype(JSContext *ctx, JSValue *value,
                                    JSValue *prototype) const {
  auto object = value->getData()->cast<JSObject>();
  auto old = object->getPrototype();
  if (old) {
    value->getAtom()->removeChild(old);
    ctx->recycle(old);
  }
  object->setPrototype(prototype->getAtom());
  value->getAtom()->addChild(prototype->getAtom());
  return prototype;
}

JSValue *JSObjectType::getPrototypeOf(JSContext *ctx, JSValue *value) const {
  auto object = value->getData()->cast<JSObject>();
  auto prototype = object->getPrototype();
  if (prototype) {
    return ctx->createValue(prototype);
  } else {
    return ctx->createNull();
  }
}
JSValue *JSObjectType::setConstructor(JSContext *ctx, JSValue *value,
                                      JSValue *constructor) const {
  auto object = value->getData()->cast<JSObject>();
  auto old = object->getConstructor();
  if (old) {
    value->getAtom()->removeChild(old);
    ctx->recycle(old);
  }
  object->setConstructor(constructor->getAtom());
  value->getAtom()->addChild(constructor->getAtom());
  return constructor;
}

JSValue *JSObjectType::getConstructorOf(JSContext *ctx, JSValue *value) const {
  auto object = value->getData()->cast<JSObject>();
  auto constructor = object->getConstructor();
  if (constructor) {
    return ctx->createValue(constructor);
  }
  return ctx->createUndefined();
}

JSValue *JSObjectType::getKeys(JSContext *ctx, JSValue *value) const {
  auto arr = ctx->createArray();
  ctx->pushScope();
  auto obj = value->getData()->cast<JSObject>();
  std::vector<std::wstring> keys;
  while (obj) {
    auto &fields = obj->getFields();
    for (auto &[keyAtom, field] : fields) {
      auto key = ctx->createValue(keyAtom);
      if (field.enumable && key->isTypeof<JSStringType>()) {
        auto keystr = ctx->checkedString(key);
        if (std::find(keys.begin(), keys.end(), keystr) == keys.end()) {
          keys.push_back(keystr);
        }
      }
    }
    obj = obj->getPrototype()->getData()->cast<JSObject>();
  }
  size_t index = 0;
  for (auto it = keys.rbegin(); it != keys.rend(); it++) {
    auto err =
        ctx->setField(arr, ctx->createNumber(index), ctx->createString(*it));
    CHECK(ctx, err);
    index++;
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
  name = ctx->clone(name);
  if (!name->isTypeof<JSStringType>() && !name->isTypeof<JSSymbolType>()) {
    name = ctx->toString(name);
  }
  CHECK(ctx, name);
  std::wstring fieldname = ctx->checkedString(name);
  auto obj = object->getData()->cast<JSObject>();
  JSField *pfield = nullptr;
  auto &fields = obj->getFields();
  for (auto &[keyAtom, field] : fields) {
    auto key = ctx->createValue(keyAtom);
    auto res = ctx->isEqual(key, name);
    CHECK(ctx, res);
    if (ctx->checkedBoolean(res)) {
      pfield = &field;
      break;
    }
  }
  if (pfield) {
    if (pfield->value != nullptr) {
      auto oldvalue = ctx->createValue(pfield->value);
      if (oldvalue->getType() != value->getType() ||
          !ctx->checkedBoolean(ctx->isEqual(oldvalue, value))) {
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
          ctx->recycle(oldvalue->getAtom());
        }
      }
    } else {
      if (!pfield->setter) {
        return ctx->createException(
            JSException::TYPE::TYPE,
            std::format(L"Cannot add property {}, object is not extensible",
                        fieldname));
      }
      auto setter = ctx->createValue(pfield->setter);
      auto err = ctx->call(setter, object, {value});
      CHECK(ctx, err);
    }
  } else {
    auto err = defineProperty(ctx, object, name, value);
    CHECK(ctx, err);
  }
  ctx->popScope();
  return object;
}

JSValue *JSObjectType::getField(JSContext *ctx, JSValue *object,
                                JSValue *name) const {
  auto current = ctx->getScope();
  if (!name->isTypeof<JSStringType>() && !name->isTypeof<JSSymbolType>()) {
    name = ctx->toString(name);
  }
  JSValue *result = nullptr;
  JSField *field = getFieldDescriptor(ctx, object, name);
  if (field) {
    ctx->pushScope();
    if (field->value) {
      result = current->createValue(field->value);
    } else {
      if (!field->getter) {
        return ctx->createUndefined();
      }
      auto getter = ctx->createValue(field->getter);
      auto res = ctx->call(getter, object, {});
      if (res->isTypeof<JSExceptionType>()) {
        return res;
      }
      result = current->createValue(res->getAtom());
    }
    ctx->popScope();
    return result;
  } else {
    return ctx->createUndefined();
  }
}

JSValue *JSObjectType::setPrivateField(JSContext *ctx, JSValue *object,
                                       const std::wstring &name,
                                       JSValue *value) const {
  ctx->pushScope();
  auto obj = object->getData()->cast<JSObject>();
  JSField *field = nullptr;
  auto &fields = obj->getPrivateFields();
  if (!fields.contains(name)) {
    return ctx->createException(
        JSException::TYPE::SYNTAX,
        std::format(
            L"Private field '#{}' must be declared in an enclosing class",
            name));
  }
  field = &fields.at(name);
  if (field->value != nullptr) {
    auto oldvalue = ctx->createValue(field->value);
    if (oldvalue->getType() != value->getType() ||
        !ctx->checkedBoolean(ctx->isEqual(oldvalue, value))) {
      if (!field->writable || obj->isFrozen()) {
        return ctx->createException(
            JSException::TYPE::TYPE,
            std::format(L"Cannot assign to read only property '{}' of object "
                        L"'#<Object>'",
                        name));
      } else {
        object->getAtom()->addChild(value->getAtom());
        object->getAtom()->removeChild(oldvalue->getAtom());
        field->value = value->getAtom();
        ctx->recycle(oldvalue->getAtom());
      }
    }
  } else {
    if (!field->setter) {
      return ctx->createException(
          JSException::TYPE::TYPE,
          std::format(L"Cannot add property {}, object is not extensible",
                      name));
    }
    auto setter = ctx->createValue(field->setter);
    auto err = ctx->call(setter, object, {value});
    if (err->isTypeof<JSExceptionType>()) {
      return err;
    }
  }
  ctx->popScope();
  return object;
}

JSValue *JSObjectType::getPrivateField(JSContext *ctx, JSValue *object,
                                       const std::wstring &name) const {
  auto current = ctx->getScope();
  auto obj = object->getData()->cast<JSObject>();
  JSValue *result = nullptr;
  JSField *field = nullptr;
  if (obj->getPrivateFields().contains(name)) {
    field = &obj->getPrivateFields().at(name);
  }
  if (field) {
    ctx->pushScope();
    if (field->value) {
      result = current->createValue(field->value);
    } else {
      if (!field->getter) {
        return ctx->createUndefined();
      }
      auto getter = ctx->createValue(field->getter);
      auto res = ctx->call(getter, object, {});
      if (res->isTypeof<JSExceptionType>()) {
        return res;
      }
      result = current->createValue(res->getAtom());
    }
    ctx->popScope();
    return result;
  } else {
    return ctx->createException(
        JSException::TYPE::SYNTAX,
        std::format(
            L"Private field '#{}' must be declared in an enclosing class",
            name));
  }
}

JSValue *JSObjectType::defineProperty(JSContext *ctx, JSValue *object,
                                      JSValue *name, JSValue *value,
                                      bool configurable, bool enumable,
                                      bool writable) const {

  auto obj = object->getData()->cast<JSObject>();
  ctx->pushScope();
  name = ctx->clone(name);
  if (!name->isTypeof<JSStringType>() && !name->isTypeof<JSSymbolType>()) {
    name = ctx->toString(name);
  }
  CHECK(ctx, name);
  std::wstring fieldname = ctx->checkedString(name);
  JSField *field = getOwnFieldDescriptor(ctx, object, name);
  if (field) {
    if (!field->configurable &&
        (field->configurable != configurable || field->enumable != enumable ||
         field->setter == nullptr)) {
      return ctx->createException(
          JSException::TYPE::TYPE,
          std::format(L"Cannot assign to read only property '{}' of object "
                      L"'#<Object>'",
                      fieldname));
    }
    auto oldvalue = ctx->createValue(field->value);
    if (oldvalue->getType() != value->getType() ||
        !ctx->checkedBoolean(ctx->isEqual(oldvalue, value))) {
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
    auto &fields = object->getData()->cast<JSObject>()->getFields();
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
  return value;
}

JSValue *JSObjectType::defineProperty(JSContext *ctx, JSValue *object,
                                      JSValue *name, JSValue *getter,
                                      JSValue *setter, bool configurable,
                                      bool enumable) const {
  auto obj = object->getData()->cast<JSObject>();
  ctx->pushScope();
  if (!name->isTypeof<JSStringType>() && !name->isTypeof<JSSymbolType>()) {
    name = ctx->toString(name);
  }
  CHECK(ctx, name);
  std::wstring fieldname = ctx->checkedString(name);
  JSField *field = getOwnFieldDescriptor(ctx, object, name);
  if (field) {
    if (!field->configurable &&
        (field->configurable != configurable || field->enumable != enumable ||
         field->value != nullptr)) {
      return ctx->createException(
          JSException::TYPE::TYPE,
          std::format(L"Cannot assign to read only property '{}' of object "
                      L"'#<Object>'",
                      fieldname));
    }
    field->configurable = configurable;
    field->enumable = enumable;
    if (getter) {
      object->getAtom()->addChild(getter->getAtom());
    }
    if (setter) {
      object->getAtom()->addChild(setter->getAtom());
    }
    if (field->getter) {
      object->getAtom()->removeChild(field->getter);
    }
    if (field->setter) {
      object->getAtom()->removeChild(field->setter);
    }
    field->getter = getter == nullptr ? nullptr : getter->getAtom();
    field->setter = setter == nullptr ? nullptr : setter->getAtom();
  } else {
    if (obj->isSealed() || !obj->isExtensible() || obj->isFrozen()) {
      return ctx->createException(
          JSException::TYPE::TYPE,
          std::format(L"Cannot assign to read only property '{}' of object "
                      L"'#<Object>'",
                      fieldname));
    }
    auto pname = ctx->clone(name);
    object->getAtom()->addChild(pname->getAtom());
    if (getter) {
      object->getAtom()->addChild(getter->getAtom());
    }
    if (setter) {
      object->getAtom()->addChild(setter->getAtom());
    }
    object->getData()->cast<JSObject>()->getFields()[pname->getAtom()] = {
        .configurable = configurable,
        .enumable = enumable,
        .value = nullptr,
        .writable = true,
        .getter = getter == nullptr ? nullptr : getter->getAtom(),
        .setter = setter == nullptr ? nullptr : setter->getAtom(),
    };
  }
  ctx->popScope();
  return object;
}

JSValue *JSObjectType::definePrivateProperty(JSContext *ctx, JSValue *object,
                                             const std::wstring &name,
                                             JSValue *value) const {
  auto obj = object->getData()->cast<JSObject>();
  ctx->pushScope();
  if (obj->getPrivateFields().contains(name)) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        std::format(L"Cannot create duplicate private field:'{}' to object",
                    name));
  } else {
    if (obj->isSealed() || !obj->isExtensible() || obj->isFrozen()) {
      return ctx->createException(
          JSException::TYPE::TYPE,
          std::format(L"Cannot assign to read only property '{}' of object "
                      L"'#<Object>'",
                      name));
    }
    object->getAtom()->addChild(value->getAtom());
    auto &fields = obj->getPrivateFields();
    fields[name] = {
        .configurable = false,
        .enumable = false,
        .value = value->getAtom(),
        .writable = true,
        .getter = nullptr,
        .setter = nullptr,
    };
  }
  ctx->popScope();
  return object;
}

JSValue *JSObjectType::definePrivateProperty(JSContext *ctx, JSValue *object,
                                             const std::wstring &name,
                                             JSValue *getter,
                                             JSValue *setter) const {
  auto obj = object->getData()->cast<JSObject>();
  ctx->pushScope();
  if (obj->getPrivateFields().contains(name)) {
    return ctx->createException(
        JSException::TYPE::TYPE,
        std::format(L"Cannot create duplicate private field:'{}' to object",
                    name));
  } else {
    if (obj->isSealed() || !obj->isExtensible() || obj->isFrozen()) {
      return ctx->createException(
          JSException::TYPE::TYPE,
          std::format(L"Cannot assign to read only property '{}' of object "
                      L"'#<Object>'",
                      name));
    }
    if (getter) {
      object->getAtom()->addChild(getter->getAtom());
    }
    if (setter) {
      object->getAtom()->addChild(setter->getAtom());
    }
    auto &fields = obj->getPrivateFields();
    fields[name] = {
        .configurable = false,
        .enumable = false,
        .value = nullptr,
        .writable = false,
        .getter = getter != nullptr ? getter->getAtom() : nullptr,
        .setter = setter != nullptr ? setter->getAtom() : nullptr,
    };
  }
  ctx->popScope();
  return object;
}