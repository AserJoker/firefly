
#include "script/engine/JSFunctionType.hpp"
#include "script/engine/JSCallableType.hpp"
#include "script/engine/JSFunction.hpp"
#include "script/engine/JSValue.hpp"
#include "script/engine/JSVirtualMachine.hpp"
JSFunctionType::JSFunctionType(JSAllocator *allocator)
    : JSCallableType(allocator) {}
JSValue *JSFunctionType::call(JSContext *ctx, JSValue *func, JSValue *self,
                              std::vector<JSValue *> args) const {
  auto current = ctx->getScope();
  auto fn = func->getData()->cast<JSFunction>();

  auto runtime = ctx->getRuntime();
  auto vm = runtime->getVirtualMachine();
  auto program = runtime->getProgram(fn->getProgramPath());
  if (fn->getSelf()) {
    self = ctx->createValue(fn->getSelf());
  }
  JSValue *clazz = nullptr;
  if (fn->getClass()) {
    clazz = ctx->createValue(fn->getClass());
  }
  std::vector<JSValue *> arguments = {args.rbegin(), args.rend()};
  auto res = vm->eval(ctx, program,
                      {
                          .pc = fn->getAddress(),
                          .stack = arguments,
                          .self = self,
                          .clazz = clazz,
                      });
  res = current->createValue(res->getAtom());
  while (ctx->getScope() != current) {
    ctx->popScope();
  }
  return res;
}

JSValue *JSFunctionType::setSelf(JSContext *ctx, JSValue *value,
                                 JSValue *self) {
  auto fn = value->getData()->cast<JSFunction>();
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

JSValue *JSFunctionType::getSelf(JSContext *ctx, JSValue *value) {
  auto fn = value->getData()->cast<JSFunction>();
  auto self = fn->getSelf();
  if (self) {
    return ctx->createValue(self);
  }
  return ctx->createUndefined();
}

JSValue *JSFunctionType::setClass(JSContext *ctx, JSValue *value,
                                  JSValue *clazz) {
  auto fn = value->getData()->cast<JSFunction>();
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

JSValue *JSFunctionType::getClass(JSContext *ctx, JSValue *value) {
  auto fn = value->getData()->cast<JSFunction>();
  auto clazz = fn->getClass();
  if (clazz) {
    return ctx->createValue(clazz);
  }
  return ctx->createUndefined();
}