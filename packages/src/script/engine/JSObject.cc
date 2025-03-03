#include "script/engine/JSObject.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSNull.hpp"
JSObject::JSObject(JSAllocator *allocator, const JS_TYPE &type)
    : JSBase(allocator, type), _sealed(false), _frozen(false),
      _extensible(true), _prototype(nullptr), _constructor(nullptr) {}
JSBase *JSObject ::toString() {
  if (_prototype->getData()->cast<JSNull>()) {
    return getAllocator()->create<JSString>(L"[Object: null prototype] {}");
  } else {
    auto func = _constructor->getData()->cast<JSCallable>();
    return getAllocator()->create<JSString>(
        std::format(L"[{} object]", func->getName()));
  }
}

JSBase *JSObject ::clone() { return this; }

JSValue *JSObject ::getField(JSContext *ctx, JSValue *self,
                             const std::wstring &name) {
  if (_fields.contains(name)) {
    auto &field = _fields.at(name);
    if (field.value) {
      return ctx->getScope()->createValue(field.value);
    } else {
      auto fn = ctx->getScope()->createValue(field.getter);
      return ctx->call(fn, self, {});
    }
  }
  return ctx->createUndefined();
}

JSValue *JSObject ::setField(JSContext *ctx, JSValue *self,
                             const std::wstring &name, JSValue *value) {
  if (_fields.contains(name)) {
    auto &field = _fields.at(name);
    if (field.value != nullptr) {
      if (!field.writable || isFrozen()) {
        return ctx->createException(JSException::TYPE::TYPE,
                                    L"Cannot assign to read only property '" +
                                        name + L"' of object");
      }
      self->getAtom()->addChild(value->getAtom());
      self->getAtom()->removeChild(field.value);
      field.value = value->getAtom();
    } else {
      if (!field.setter) {
        return ctx->createException(JSException::TYPE::TYPE,
                                    L"Cannot assign to read only property '" +
                                        name + L"' of object");
      }
      auto fn = ctx->getScope()->createValue(field.setter);
      return ctx->call(fn, self, {value});
    }
  } else {
    if (!isExtensible() || isFrozen() || isSealed()) {
      return ctx->createException(JSException::TYPE::REFERENCE,
                                  L"Cannot add property " + name +
                                      L", object is not extensible");
    }
    self->getAtom()->addChild(value->getAtom());
    _fields[name] = {.configurable = true,
                     .enumable = true,
                     .value = value->getAtom(),
                     .writable = true};
  }
  return nullptr;
}

JSValue *JSObject::getField(JSContext *ctx, JSValue *self, JSValue *name) {
  if (name->getType() == JS_TYPE::STRING) {
    auto str = dynamic_cast<JSString *>(name->getData())->getValue();
    return getField(ctx, self, str);
  } else if (name->getType() == JS_TYPE::NUMBER) {
    auto val = dynamic_cast<JSNumber *>(name->getData())->getValue();
    return getIndex(ctx, self, (size_t)val);
  }
  if (name->getType() == JS_TYPE::SYMBOL) {
    if (_symboledFields.contains(name->getAtom())) {
      auto &field = _symboledFields.at(name->getAtom());
      if (field.value) {
        return ctx->getScope()->createValue(field.value);
      } else {
        auto fn = ctx->getScope()->createValue(field.getter);
        return ctx->call(fn, self, {});
      }
    }
  }
  return getField(ctx, self, ctx->checkedString(ctx->toString(name)));
}

JSValue *JSObject::setField(JSContext *ctx, JSValue *self, JSValue *name,
                            JSValue *value) {
  if (name->getType() == JS_TYPE::STRING) {
    auto str = dynamic_cast<JSString *>(name->getData())->getValue();
    return setField(ctx, self, str, value);
  } else if (name->getType() == JS_TYPE::NUMBER) {
    auto idx = dynamic_cast<JSNumber *>(name->getData())->getValue();
    return setIndex(ctx, self, (size_t)idx, value);
  }
  if (name->getType() == JS_TYPE::SYMBOL) {
    auto namestr = ctx->checkedString(ctx->toString(name));
    namestr = L"Symbol(" + namestr + L")";
    if (_symboledFields.contains(name->getAtom())) {
      auto &field = _symboledFields.at(name->getAtom());
      if (field.value != nullptr) {
        if (!field.writable || isFrozen()) {
          return ctx->createException(JSException::TYPE::TYPE,
                                      L"Cannot assign to read only property '" +
                                          namestr + L"' of object");
        }
        self->getAtom()->addChild(value->getAtom());
        self->getAtom()->removeChild(field.value);
        field.value = value->getAtom();
      } else {
        if (!field.setter) {
          return ctx->createException(JSException::TYPE::TYPE,
                                      L"Cannot assign to read only property '" +
                                          namestr + L"' of object");
        }
        auto fn = ctx->getScope()->createValue(field.setter);
        return ctx->call(fn, self, {value});
      }
    } else {
      if (!isExtensible() || isFrozen() || isSealed()) {
        return ctx->createException(JSException::TYPE::REFERENCE,
                                    L"Cannot add property " + namestr +
                                        L", object is not extensible");
      }
      self->getAtom()->addChild(value->getAtom());
      self->getAtom()->addChild(name->getAtom());
      _symboledFields[name->getAtom()] = {.configurable = true,
                                          .enumable = true,
                                          .value = value->getAtom(),
                                          .writable = true};
    }
  }
  return setField(ctx, self, ctx->checkedString(ctx->toString(name)), value);
}

JSValue *JSObject::getIndex(JSContext *ctx, JSValue *self, size_t index) {
  return getField(ctx, self, std::to_wstring(index));
}

JSValue *JSObject::setIndex(JSContext *ctx, JSValue *self, size_t index,
                            JSValue *value) {
  return setField(ctx, self, std::to_wstring(index), value);
}