#include "script/lib/Module_Co.hpp"
#include "core/AutoPtr.hpp"
#include "core/Coroutine.hpp"
#include "core/Promise.hpp"
#include "script/Value.hpp"
#include "script/helper/Trait_Promise.hpp"
using namespace firefly;
using namespace firefly::script;
FUNC_DEF(Module_Co::start) {
  static uint32_t count = 0;
  auto callback = args[0];
  auto coroutines = ctx->getNativeGlobal()->getField(ctx, "coroutines");
  auto index = count++;
  coroutines->setIndex(ctx, index, callback);
  auto pro = core::Coroutine::async<core::AutoPtr<Value>>([=] {
    auto _ctx = ctx;
    auto coroutines = _ctx->getNativeGlobal()->getField(ctx, "coroutines");
    auto _callback = coroutines->getIndex(ctx, index);
    auto res = _callback->call(ctx, {});
    auto arr = _ctx->createValue()->setArray(ctx);
    for (uint32_t i = 0; i < (uint32_t)res.size(); i++) {
      arr->setIndex(ctx, i, res[i]);
    }
    return arr;
  });
  auto res = Trait_Promise::create(ctx, pro);
  res->setField(ctx, "exec", callback);
  return {Trait_Promise::create(ctx, pro)};
}
FUNC_DEF(Module_Co::yield) {
  core::Coroutine::yield();
  return {};
}
FUNC_DEF(Module_Co::createPromise) {
  core::AutoPtr pro = new core::Promise<core::AutoPtr<Value>>();
  return {Trait_Promise::create(ctx, pro)};
}
void Module_Co::open(core::AutoPtr<Script> ctx) {
  auto exports = ctx->createValue()->setObject(ctx);
  exports->setFunctionField(ctx, createPromise);
  exports->setFunctionField(ctx, start);
  exports->setFunctionField(ctx, yield);
  ctx->getNativeGlobal()->setField(ctx, "coroutines",
                                   ctx->createValue()->setArray(ctx));
  ctx->registerModule("co", exports);
}