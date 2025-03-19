#include "script/engine/JSNativeFunctionType.hpp"
#include "script/engine/JSNativeFunction.hpp"
JSNativeFunctionType::JSNativeFunctionType(JSAllocator *allocator)
    : JSCallableType(allocator) {}
    
JSValue *JSNativeFunctionType::call(JSContext *ctx, JSValue *func,
                                    JSValue *self,
                                    std::vector<JSValue *> args) const {
  auto fn = func->getData()->cast<JSNativeFunction>();
  return fn->getNative()(ctx, self, args);
}