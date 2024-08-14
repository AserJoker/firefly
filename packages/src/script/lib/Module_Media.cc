#include "script/lib/Module_Media.hpp"
#include "core/Buffer.hpp"
#include "core/Singleton.hpp"
#include "runtime/Media.hpp"
#include "script/Atom.hpp"
#include "script/Script.hpp"
#include "script/helper/Trait_Buffer.hpp"
#include "script/helper/Trait_Resource.hpp"
using namespace firefly;
using namespace firefly::script;
void Module_Media::open(core::AutoPtr<Script> ctx) {
  auto exports = ctx->createValue()->setObject(ctx);
  exports->setFunctionField(ctx, &query)->setFunctionField(ctx, createBuffer);
  ctx->registerModule("media", exports);
}
FUNC_DEF(Module_Media::query) {
  auto media = core::Singleton<runtime::Media>::instance();
  auto resource = media->load(args[0]->toString(ctx));
  return {Trait_Resource::create(ctx, resource)};
}
FUNC_DEF(Module_Media::createBuffer) {
  if (args[0]->getType(ctx) == Atom::TYPE::NUMBER) {
    return {Trait_Buffer::create(
        ctx, new core::Buffer((uint32_t)args[0]->toNumber(ctx)))};
  } else {
    auto str = args[0]->toString(ctx);
    return {Trait_Buffer::create(
        ctx, new core::Buffer(str.size(), (void *)str.c_str()))};
  }
}