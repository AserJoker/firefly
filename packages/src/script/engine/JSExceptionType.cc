#include "script/engine/JSExceptionType.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSException.hpp"
#include "script/engine/JSValue.hpp"

JSExceptionType::JSExceptionType(JSAllocator *allocator) : JSType(allocator) {}

const wchar_t *JSExceptionType::getTypeName() const { return L"internal"; }
JSValue *JSExceptionType::toString(JSContext *ctx, JSValue *value) const {
  auto e = value->getData()->cast<JSException>();
  if (e->getValue()) {
    auto evalue = ctx->createValue(e->getValue());
    return ctx->toString(evalue);
  }
  std::wstringstream ss;
  ss << getTypeName() << L": ";
  ss << e->getMessage() << std::endl;
  for (auto it = e->getStack().rbegin(); it != e->getStack().rend(); it++) {
    auto funcname = it->position.funcname;
    if (funcname.empty()) {
      funcname = L"anonymous";
    }
    if (it->filename.empty()) {
      ss << L"  at " << funcname << L"(<internal>)";
    } else {
      ss << L"  at " << funcname << L"(" << it->filename << L":"
         << it->position.line << L":" << it->position.column << L")\n";
    }
  }
  return ctx->createString(ss.str());
}

JSValue *JSExceptionType::toNumber(JSContext *ctx, JSValue *value) const {
  return ctx->createNaN();
}

JSValue *JSExceptionType::toBoolean(JSContext *ctx, JSValue *value) const {
  return ctx->createBoolean(true);
}

JSValue *JSExceptionType::clone(JSContext *ctx, JSValue *value) const {
  return ctx->createValue(value);
}
JSValue *JSExceptionType::pack(JSContext *ctx, JSValue *value) const {
  return value;
};
JSValue *JSExceptionType::equal(JSContext *ctx, JSValue *value,
                                JSValue *another) const {
  return ctx->createBoolean(value->getData() == another->getData());
}