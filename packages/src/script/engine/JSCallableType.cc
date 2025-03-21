#include "script/engine/JSCallableType.hpp"
#include "script/engine/JSCallable.hpp"
#include "script/engine/JSObjectType.hpp"
#include "script/util/JSAllocator.hpp"
JSCallableType::JSCallableType(JSAllocator *allocator)
    : JSObjectType(allocator) {}
const wchar_t *JSCallableType::getTypeName() const { return L"function"; }

JSValue *JSCallableType::setSelf(JSContext *ctx, JSValue *value,
                                 JSValue *self) const {
  auto fn = value->getData()->cast<JSCallable>();
  auto old = fn->getSelf();
  if (old) {
    ctx->recycle(old);
  }
  if (self) {
    value->getAtom()->addChild(self->getAtom());
    fn->setSelf(self->getAtom());
  }
  return ctx->createUndefined();
}

JSValue *JSCallableType::getSelf(JSContext *ctx, JSValue *value) const {
  auto fn = value->getData()->cast<JSCallable>();
  auto self = fn->getSelf();
  if (self) {
    return ctx->createValue(self);
  }
  return ctx->createUndefined();
}

JSValue *JSCallableType::setClass(JSContext *ctx, JSValue *value,
                                  JSValue *clazz) const {
  auto fn = value->getData()->cast<JSCallable>();
  auto old = fn->getClass();
  if (old) {
    ctx->recycle(old);
  }
  if (clazz) {
    value->getAtom()->addChild(clazz->getAtom());
    fn->setClass(clazz->getAtom());
  }
  return ctx->createUndefined();
}

JSValue *JSCallableType::getClass(JSContext *ctx, JSValue *value) const {
  auto fn = value->getData()->cast<JSCallable>();
  auto clazz = fn->getClass();
  if (clazz) {
    return ctx->createValue(clazz);
  }
  return ctx->createUndefined();
}