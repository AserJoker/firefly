#include "script/lib/Module_Array.hpp"
#include "core/AutoPtr.hpp"
#include "script/Atom.hpp"
#include "script/Script.hpp"
using namespace firefly;
using namespace firefly::script;
void Module_Array::open(core::AutoPtr<Script> ctx) {
  auto exports = ctx->createValue()->setObject(ctx);
  exports->setFunctionField(ctx, createArray);
  ctx->registerModule("array", exports);
}

FUNC_DEF(Module_Array::createArray) {
  auto arr = ctx->createValue()->setArray(ctx);
  if (args.size()) {
    if (args[0]->getType(ctx) == Atom::TYPE::NUMBER) {
      uint32_t size = (uint32_t)args[0]->toNumber(ctx);
      for (uint32_t i = 0; i < size; i++) {
        arr->setIndex(ctx, i, ctx->createValue());
      }
    } else if (args[0]->getType(ctx) == Atom::TYPE::ARRAY) {
      return {args[0]};
    } else if (args[0]->getType(ctx) == Atom::TYPE::OBJECT) {
      auto len = args[0]->getLength(ctx);
      for (uint32_t i = 0; i < len; i++) {
        arr->setIndex(ctx, i, args[0]->getIndex(ctx, i));
      }
    }
  }
  return {arr};
}