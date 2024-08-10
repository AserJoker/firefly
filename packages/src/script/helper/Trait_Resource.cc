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
  auto size = 0;
  if (args.size() > 1) {
    size = args[1]->toNumber(ctx);
  }
  auto buffer = resource->read(size);
  return {Trait_Buffer::create(ctx, buffer)};
}
core::AutoPtr<Value>
Trait_Resource::create(core::AutoPtr<Script> ctx,
                       core::AutoPtr<runtime::Resource> resource) {
  auto obj = ctx->createValue()->setObject(ctx)->setOpaque(resource)->setField(
      ctx, "read", ctx->createValue()->setFunction(ctx, read));
  return obj;
}