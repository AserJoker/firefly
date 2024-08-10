#include "script/lib/Module_Media.hpp"
#include "core/Singleton.hpp"
#include "runtime/Media.hpp"
#include "script/Script.hpp"
#include "script/helper/Trait_Resource.hpp"
using namespace firefly;
using namespace firefly::script;
void Module_Media::open(core::AutoPtr<Script> ctx) {
  auto exports = ctx->createValue()->setObject(ctx);
  exports->setField(ctx, "query", ctx->createValue()->setFunction(ctx, &query));
  ctx->registerModule("media", exports);
}
FUNC_DEF(Module_Media::query) {
  auto media = core::Singleton<runtime::Media>::instance();
  auto resource = media->load(args[0]->toString(ctx));
  return {Trait_Resource::create(ctx, resource)};
}