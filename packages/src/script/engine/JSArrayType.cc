#include "script/engine/JSArrayType.hpp"
#include "script/engine/JSArray.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSObjectType.hpp"
#include "script/engine/JSValue.hpp"
#include "script/util/JSAllocator.hpp"
#include <sstream>

JSArrayType ::JSArrayType(JSAllocator *allocator) : JSObjectType(allocator) {}

JSValue *JSArrayType::toString(JSContext *ctx, JSValue *value) const {
  std::wstringstream ss;
  auto arr = value->getData()->cast<JSArray>();
  auto &items = arr->getItems();
  ctx->popScope();
  for (size_t index = 0; index < arr->getLength(); index++) {
    if (items.contains(index)) {
      auto str = ctx->toString(ctx->getScope()->createValue(items[index]));
      ss << ctx->checkedString(str);
    }
    if (index != arr->getLength() - 1) {
      ss << L",";
    }
  }
  ctx->popScope();
  return ctx->createString(ss.str());
}
