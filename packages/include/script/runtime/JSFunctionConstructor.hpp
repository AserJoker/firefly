#pragma once
#include "../engine/JSContext.hpp"
class JSFunctionConstructor {
public:
  static JSValue *constructor(JSContext *ctx, JSValue *_,
                              std::vector<JSValue *> args) {
    if (args.empty()) {
      ctx->getRuntime()->compile(L"", L"", JS_EVAL_TYPE::FUNCTION);
      return ctx->createFunction(L"", L"", 0);
    }

    std::wstring argument;
    if (args.size() > 1) {
      for (size_t index = 0; index < args.size() - 1; index++) {
        auto str = ctx->toString(args[index]);
        if (str->getType() == JS_TYPE::EXCEPTION) {
          return str;
        }
        argument += ctx->checkedString(str);
        if (index != args.size() - 2) {
          argument += L",";
        }
      }
    }
    auto str = ctx->toString(args[args.size() - 1]);
    if (str->getType() == JS_TYPE::EXCEPTION) {
      return str;
    }
    auto body = ctx->checkedString(str);
    std::wstring source = std::format(L"(function ({}){{{}}})", argument, body);
    auto current = ctx->setScope(
        ctx->getAllocator()->create<JSScope>(ctx->getRootScope()));
    auto val = ctx->eval(source, source, JS_EVAL_TYPE::FUNCTION);
    auto callable = val->getData()->cast<JSCallable>();
    callable->setGlobalContext(true);
    auto result = current->createValue(val->getAtom());
    current = ctx->setScope(current);
    ctx->getAllocator()->dispose(current);
    return result;
  }
  static JSValue *initialize(JSContext *ctx) {
    auto global = ctx->getGlobal();
    auto prototype = ctx->createObject();
    auto constructor = ctx->getAllocator()->create<JSNativeFunction>(
        L"Function", &JSFunctionConstructor::constructor);
    auto Function = ctx->getScope()->createValue(constructor);
    constructor->setPrototype(prototype->getAtom());
    prototype->getAtom()->addChild(Function->getAtom());
    auto err = ctx->setField(Function, L"constructor", Function);
    if (err) {
      return err;
    }
    err = ctx->setField(prototype, L"constructor", Function);
    if (err) {
      return err;
    }
    err = ctx->setField(Function, L"prototype", prototype);
    if (err) {
      return err;
    }
    err = ctx->setField(global, L"Function", Function);
    if (err) {
      return err;
    }
    err = ctx->setField(Function, L"name", ctx->createString(L"Function"));
    if (err) {
      return err;
    }
    return nullptr;
  }
};