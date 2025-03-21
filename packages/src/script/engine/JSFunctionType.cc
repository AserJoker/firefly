
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
  std::vector<JSValue *> arguments = {args.rbegin(), args.rend()};
  auto res = vm->eval(ctx, program,
                      {
                          .pc = fn->getAddress(),
                          .stack = arguments,
                          .self = self,
                      });
  res = current->createValue(res->getAtom());
  while (ctx->getScope() != current) {
    ctx->popScope();
  }
  return res;
}