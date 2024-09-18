#include "script/helper/Trait_Promise.hpp"
#include "core/AutoPtr.hpp"
#include "core/Coroutine.hpp"
#include "core/Promise.hpp"
#include "exception/ValidateException.hpp"
#include "script/Atom.hpp"
#include "script/Script.hpp"
#include "script/Value.hpp"
#include <exception>
using namespace firefly;
using namespace firefly::script;
using exception::ValidateException;
core::AutoPtr<Value>
Trait_Promise::create(core::AutoPtr<Script> ctx,
                      core::Promisify<core::AutoPtr<Value>> pro) {
  auto global = ctx->getNativeGlobal();
  auto Buffer = global->getField(ctx, "Promise");
  auto instance = ctx->createValue()->setObject(ctx)->setOpaque(pro);
  for (auto &field : Buffer->getKeys(ctx)) {
    instance->setField(ctx, field, Buffer->getField(ctx, field));
  }
  instance->setField(ctx, "callbacks", ctx->createValue()->setArray(ctx));
  return instance;
}

void Trait_Promise::initialize(core::AutoPtr<Script> ctx) {
  auto global = ctx->getNativeGlobal();

  auto Promise = ctx->createValue()
                     ->setObject(ctx)
                     ->setFunctionField(ctx, wait)
                     ->setFunctionField(ctx, next)
                     ->setFunctionField(ctx, resolve)
                     ->setFunctionField(ctx, reject);

  global->setField(ctx, "Promise", Promise);
}
FUNC_DEF(Trait_Promise::next) {
  VALIDATE_ARGS(next, 2);
  auto self = args[0];
  auto callback = args[1];
  auto pro = self->getOpaque().cast<core::Promise<core::AutoPtr<Value>>>();
  auto callbacks = self->getField(ctx, "callbacks");
  auto length = callbacks->getLength(ctx);
  callbacks->setIndex(ctx, length, callback);
  core::AutoPtr next = new core::Promise<core::AutoPtr<Value>>();
  try {
    core::Coroutine::next(
        pro,
        [=](const core::AutoPtr<Value> &value) -> void {
          auto _ctx = ctx;
          auto _next = next;
          Value::Stack res;
          auto _callbacks = callbacks;
          auto _callback = _callbacks->getIndex(ctx, length);
          try {
            res = _callback->call(ctx, {value});
          } catch (const std::exception &e) {
            _next->reject(e.what());
            return;
          }
          if (res.empty()) {
            _next->resolve(_ctx->createValue());
          } else {
            auto arr = _ctx->createValue()->setArray(ctx);
            for (auto i = 0; i < res.size(); i++) {
              arr->setIndex(ctx, i, res[i]);
            }
            _next->resolve(arr);
          }
        },
        [=](const std::string &message) {
          auto _ctx = ctx;
          auto _next = next;
          auto _callbacks = callbacks;
          auto _callback = _callbacks->getIndex(ctx, length);
          if (_callback->getType(ctx) != Atom::TYPE::NIL) {
            try {
              _callback->call(
                  ctx, {_ctx->createValue(), _ctx->createValue()->setString(
                                                 ctx, pro->getMessage())});
              _next->resolve(_ctx->createValue()->setArray(ctx));
            } catch (const std::exception &e) {
              _next->reject(e.what());
              return;
            }
          } else {
            _next->reject(pro->getMessage());
          }
        });
  } catch (const std::exception &e) {
    callback->call(ctx, {ctx->createValue(),
                         ctx->createValue()->setString(ctx, e.what())});
    next->reject(e.what());
  }
  return {create(ctx, next)};
}
FUNC_DEF(Trait_Promise::wait) {
  VALIDATE_ARGS(wait, 1);
  auto self = args[0];
  auto pro = self->getOpaque().cast<core::Promise<core::AutoPtr<Value>>>();
  auto res = core::Coroutine::wait(pro);
  return {res};
}
FUNC_DEF(Trait_Promise::resolve) {
  VALIDATE_ARGS(resolve, 1);
  auto self = args[0];
  auto pro = self->getOpaque().cast<core::Promise<core::AutoPtr<Value>>>();
  core::AutoPtr<Value> value = ctx->createValue()->setArray(ctx);
  for (uint32_t i = 0; i < (uint32_t)args.size() - 1; i++) {
    value->setIndex(ctx, i, args[i + 1]);
  }
  self->setField(ctx, "value", value);
  pro->resolve(value);
  return {};
}
FUNC_DEF(Trait_Promise::reject) {
  VALIDATE_ARGS(reject, 2);
  auto self = args[0];
  auto pro = self->getOpaque().cast<core::Promise<core::AutoPtr<Value>>>();
  auto err = args[1];
  self->setField(ctx, "message", err);
  pro->reject(err->toString(ctx));
  return {};
}