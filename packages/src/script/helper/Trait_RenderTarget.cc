#include "script/helper/Trait_RenderTarget.hpp"
#include "core/AutoPtr.hpp"
#include "script/Script.hpp"
#include "script/Value.hpp"
#include "script/helper/Trait_Node.hpp"
using namespace firefly;
using namespace firefly::script;
void Trait_RenderTarget::initialize(core::AutoPtr<Script> ctx) {
  auto global = ctx->getNativeGlobal();
  auto RenderTarget = ctx->createValue()->setObject(ctx);
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