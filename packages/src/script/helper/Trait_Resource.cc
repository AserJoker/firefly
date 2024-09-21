#include "script/helper/Trait_Resource.hpp"
#include "core/AutoPtr.hpp"
#include "runtime/Resource.hpp"
#include "script/Script.hpp"
#include "script/Value.hpp"
#include "script/helper/Trait_Buffer.hpp"
using namespace firefly;
using namespace firefly::script;
FUNC_DEF(Trait_Resource::read) {
  auto self = args[0];
  auto resource = self->getOpaque().cast<runtime::Resource>();
  uint32_t size = 0;
  if (args.size() > 1) {
    size = (uint32_t)args[1]->toNumber(ctx);
  }
  auto buffer = resource->read(size);
  return {Trait_Buffer::create(ctx, buffer)};
}
void Trait_Resource::initialize(core::AutoPtr<Script> ctx) {
  auto scope = ctx->pushScope();
  auto global = ctx->getNativeGlobal();
  auto Resource = ctx->createValue()->setObject(ctx)->setField(
      ctx, "read", ctx->createValue()->setFunction(ctx, read));
  global->setField(ctx, "Resource", Resource);
  ctx->popScope(scope);
}
core::AutoPtr<Value>
Trait_Resource::create(core::AutoPtr<Script> ctx,
                       core::AutoPtr<runtime::Resource> resource) {
  auto global = ctx->getNativeGlobal();
  auto Resource = global->getField(ctx, "Resource");
  auto instance = ctx->createValue()->setObject(ctx)->setOpaque(resource);
  for (auto &field : Resource->getKeys(ctx)) {
    instance->setField(ctx, field, Resource->getField(ctx, field));
  }
  return instance;
}