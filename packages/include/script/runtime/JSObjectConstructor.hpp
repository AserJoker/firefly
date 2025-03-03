#pragma once
#include "../engine/JSContext.hpp"
class JSObjectConstructor {
public:
  static JSValue *constructor(JSContext *ctx, JSValue *self,
                              std::vector<JSValue *> args) {
    return nullptr;
  }
  static JSValue *initialize(JSContext *ctx) {
    auto Object =
        ctx->createNativeFunction(JSObjectConstructor::constructor, L"Object");
    if (Object->getType() == JS_TYPE::EXCEPTION) {
      return Object;
    }
    auto prototype = ctx->getField(Object, L"prototype");
    if (prototype->getType() == JS_TYPE::EXCEPTION) {
      return prototype;
    }
    auto err = ctx->setField(prototype, L"constructor", Object);
    if (err) {
      return err;
    }
    auto Function = ctx->getGlobal(L"Function");
    auto funcproto = ctx->getField(Function, L"prototype");
    funcproto->getData()->cast<JSObject>()->setConstructor(Object->getAtom());
    funcproto->getAtom()->addChild(Object->getAtom());
    funcproto->getData()->cast<JSObject>()->setPrototype(prototype->getAtom());
    funcproto->getAtom()->addChild(prototype->getAtom());
    auto null = ctx->createNull();
    prototype->getData()->cast<JSObject>()->setPrototype(null->getAtom());
    prototype->getAtom()->addChild(null->getAtom());
    err = ctx->setField(ctx->getGlobal(), L"Object", Object);
    if (err) {
      return err;
    }
    return nullptr;
  }
};