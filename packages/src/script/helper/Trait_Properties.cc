#include "script/helper/Trait_Properties.hpp"
#include "core/AutoPtr.hpp"
#include "core/Properties.hpp"
#include "script/Script.hpp"
using namespace firefly;
using namespace firefly::script;
using SelfType = core::AutoPtr<core::Properties>;
void Trait_Properties::initialize(core::AutoPtr<Script> ctx) {
  auto global = ctx->getNativeGlobal();
  auto Properties = ctx->createValue()
                        ->setObject(ctx)
                        ->setFunctionField(ctx, getKey)
                        ->setFunctionField(ctx, setKey)
                        ->setFunctionField(ctx, removeKey)
                        ->setFunctionField(ctx, store);
  global->setField(ctx, "Properties", Properties);
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
  auto [self, key] = Script::parseArgs<SelfType, std::string>(ctx, args);
  auto result = self->get(key);
  return {ctx->createValue(result)};
}
FUNC_DEF(Trait_Properties::setKey) {
  auto [self, key, value] =
      Script::parseArgs<SelfType, std::string, std::string>(ctx, args);
  self->set(key, value);
  return {};
}
FUNC_DEF(Trait_Properties::removeKey) {
  auto [self, key] = Script::parseArgs<SelfType, std::string>(ctx, args);
  self->del(key);
  return {};
}
FUNC_DEF(Trait_Properties::store) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  auto data = self->store();
  return {ctx->createValue(data)};
}