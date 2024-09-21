#include "script/helper/Trait_Properties.hpp"
#include "core/Properties.hpp"
using namespace firefly;
using namespace firefly::script;
void Trait_Properties::initialize(core::AutoPtr<Script> ctx) {
  auto scope = ctx->pushScope();
  auto global = ctx->getNativeGlobal();
  auto Properties = ctx->createValue()
                        ->setObject(ctx)
                        ->setFunctionField(ctx, getKey)
                        ->setFunctionField(ctx, setKey)
                        ->setFunctionField(ctx, removeKey)
                        ->setFunctionField(ctx, store);
  global->setField(ctx, "Properties", Properties);
  ctx->popScope(scope);
}
core::AutoPtr<Value>
Trait_Properties::create(core::AutoPtr<Script> ctx,
                         core::AutoPtr<core::Properties> buffer) {
  auto global = ctx->getNativeGlobal();
  auto Properties = global->getField(ctx, "Properties");
  auto instance = ctx->createValue()->setObject(ctx)->setOpaque(buffer);
  for (auto &field : Properties->getKeys(ctx)) {
    instance->setField(ctx, field, Properties->getField(ctx, field));
  }
  return instance;
}
FUNC_DEF(Trait_Properties::getKey) {
  auto self = args[0]->getOpaque().cast<core::Properties>();
  auto key = args[1]->toString(ctx);
  auto result = self->get(key);
  return {createString(ctx, result)};
}
FUNC_DEF(Trait_Properties::setKey) {
  auto self = args[0]->getOpaque().cast<core::Properties>();
  auto key = args[1]->toString(ctx);
  auto value = args[2]->toString(ctx);
  self->set(key, value);
  return {};
}
FUNC_DEF(Trait_Properties::removeKey) {
  auto self = args[0]->getOpaque().cast<core::Properties>();
  auto key = args[1]->toString(ctx);
  self->del(key);
  return {};
}
FUNC_DEF(Trait_Properties::store) {
  auto self = args[0]->getOpaque().cast<core::Properties>();
  auto data = self->store();
  return {createString(ctx, data)};
}