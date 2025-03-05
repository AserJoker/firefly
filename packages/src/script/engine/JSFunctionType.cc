
#include "script/engine/JSFunctionType.hpp"
#include "script/engine/JSCallableType.hpp"
#include "script/engine/JSFunction.hpp"
#include "script/engine/JSVirtualMachine.hpp"
JSFunctionType::JSFunctionType(JSAllocator *allocator)
    : JSCallableType(allocator) {}
JSValue *JSFunctionType::call(JSContext *ctx, JSValue *func, JSValue *self,
                              std::vector<JSValue *> args) const {
  auto fn = func->getData()->cast<JSFunction>();

  auto runtime = ctx->getRuntime();
  auto vm = runtime->getVirtualMachine();
  auto program = runtime->getProgram(fn->getProgramPath());
  return vm->eval(ctx, program,
                  {.pc = fn->getAddress(), .stack = args, .self = self});
}