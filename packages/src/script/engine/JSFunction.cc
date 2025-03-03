#include "script/engine/JSFunction.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSVirtualMachine.hpp"
JSFunction::JSFunction(
    JSAllocator *allocator, const std::wstring &name, const std::wstring &path,
    size_t address, const std::unordered_map<std::wstring, JSAtom *> &closure)
    : JSCallable(allocator, name, closure), _path(path), _address(address) {}
    
JSValue *JSFunction::call(JSContext *ctx, JSValue *self,
                          std::vector<JSValue *> args) {
  auto runtime = ctx->getRuntime();
  auto vm = runtime->getVirtualMachine();
  auto program = runtime->getProgram(_path);
  return vm->eval(ctx, program, {.pc = _address, .stack = args, .self = self});
}
