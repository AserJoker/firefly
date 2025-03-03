#pragma once
#include "../engine/JSValue.hpp"
#include "../engine/JSContext.hpp"
class JSSymbolConstructor {
public:
  static JSValue *constructor(JSContext *ctx, JSValue *self,
                              std::vector<JSValue *> args) {
    std::wstring description;
    if (!args.empty()) {
      auto str = ctx->toString(args[0]);
      if (str->getType() == JS_TYPE::EXCEPTION) {
        return str;
      }
      description = ctx->checkedString(str);
    }
    return ctx->createSymbol(description);
  }

  static JSValue *initialize(JSContext *ctx) {
    auto global = ctx->getGlobal();
    auto Symbol =
        ctx->createNativeFunction(JSSymbolConstructor::constructor, L"Symbol");
    auto err = ctx->setField(global, L"Symbol", Symbol);
    if (err) {
      return err;
    }
    return nullptr;
  }
};