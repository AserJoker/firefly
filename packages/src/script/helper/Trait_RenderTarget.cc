#include "script/helper/Trait_RenderTarget.hpp"
#include "core/AutoPtr.hpp"
#include "document/RenderTarget.hpp"
#include "script/Script.hpp"
#include "script/Value.hpp"
#include "script/helper/Trait_Node.hpp"
using namespace firefly;
using namespace firefly::script;
FUNC_DEF(Trait_RenderTarget::resize) {
  auto self = args[0]->getOpaque().cast<document::RenderTarget>();
  auto width = (uint32_t)args[1]->getField(ctx, "width")->toNumber(ctx);
  auto height = (uint32_t)args[1]->getField(ctx, "height")->toNumber(ctx);
  self->resize({width, height});
  return {};
}
void Trait_RenderTarget::initialize(core::AutoPtr<Script> ctx) {
  auto global = ctx->getNativeGlobal();
  auto RenderTarget =
      ctx->createValue()->setObject(ctx)->setFunctionField(ctx, resize);
  global->setField(ctx, "RenderTarget", RenderTarget);
}

core::AutoPtr<Value> Trait_RenderTarget::create(
    core::AutoPtr<Script> ctx,
    const core::AutoPtr<document::RenderTarget> &renderTarget) {
  auto global = ctx->getNativeGlobal();
  auto RenderTarget = global->getField(ctx, "RenderTarget");
  auto instance = Trait_Node::create(ctx, renderTarget);
  for (auto &field : RenderTarget->getKeys(ctx)) {
    instance->setField(ctx, field, RenderTarget->getField(ctx, field));
  }
  return instance;
}