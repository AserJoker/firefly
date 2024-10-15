#include "script/lib/Module_Event.hpp"
#include "core/AutoPtr.hpp"
#include "script/Atom.hpp"
#include "script/Value.hpp"
using namespace firefly;
using namespace firefly::script;
void Module_Event::open(core::AutoPtr<Script> ctx) {
  ctx->getNativeGlobal()->setField(ctx, "$events",
                                   ctx->createValue()->setObject(ctx));
  auto exports = ctx->createValue()->setObject(ctx);
  exports->setField(ctx, "on", ctx->createValue()->setFunction(ctx, on));
  exports->setField(ctx, "emit", ctx->createValue()->setFunction(ctx, emit2));
  ctx->registerModule("event", exports);
}
FUNC_DEF(Module_Event::on) {
  auto event = args[0]->toString(ctx);
  auto callback = args[1];
  auto global = ctx->getNativeGlobal();
  auto events = global->getField(ctx, "$events");
  if (events->getField(ctx, event)->getType(ctx) == Atom::TYPE::NIL) {
    events->setField(ctx, event, ctx->createValue()->setArray(ctx));
  }
  auto callbacks = events->getField(ctx, event);
  auto i = 0;
  while (callbacks->getIndex(ctx, i)->getType(ctx) != Atom::TYPE::NIL) {
    i++;
  }
  callbacks->setIndex(ctx, i, callback);
  auto off = ctx->createValue()->setFunction(
      ctx, [=](core::AutoPtr<Script> ctx, Value::Stack args) -> Value::Stack {
        auto global = ctx->getNativeGlobal();
        auto events = global->getField(ctx, "$events");
        auto callbacks = events->getField(ctx, event);
        if (callbacks->getType(ctx) == Atom::TYPE::NIL) {
          return {};
        }
        callbacks->setIndex(ctx, i, ctx->createValue());
        if (callbacks->getLength(ctx) == 0) {
          events->setField(ctx, "$events", ctx->createValue());
        }
        return {};
      });
  return {off};
}
FUNC_DEF(Module_Event::emit2) {
  auto event = args[0]->toString(ctx);
  core::AutoPtr<Value> arg;
  if (args.size() > 1) {
    arg = args[1];
  }
  emit(ctx, event, arg);
  return {};
}