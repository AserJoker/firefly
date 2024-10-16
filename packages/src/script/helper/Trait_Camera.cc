#include "script/helper/Trait_Camera.hpp"
#include "core/AutoPtr.hpp"
#include "script/Script.hpp"
#include "script/Value.hpp"
#include "video/Camera.hpp"
using namespace firefly;
using namespace firefly::script;

FUNC_DEF(Trait_Camera::setPosition) {
  auto [self, position] =
      Script::parseArgs<core::AutoPtr<video::Camera>, core::AutoPtr<Value>>(
          ctx, args);
  auto x = position->getField(ctx, "x")->toNumber(ctx);
  auto y = position->getField(ctx, "y")->toNumber(ctx);
  self->setPosition({(float)x, (float)y, 0});
  return {};
}

FUNC_DEF(Trait_Camera::getPosition) {
  auto [self] = Script::parseArgs<core::AutoPtr<video::Camera>>(ctx, args);
  auto &position = self->getPosition();
  auto result = ctx->createValue()
                    ->setObject(ctx)
                    ->setField(ctx, "x", ctx->createValue(position.x))
                    ->setField(ctx, "y", ctx->createValue(position.y));
  return {result};
}

void Trait_Camera::initialize(core::AutoPtr<Script> ctx) {
  auto global = ctx->getNativeGlobal();
  auto Camera = ctx->createValue()
                    ->setObject(ctx)
                    ->setFunctionField(ctx, getPosition)
                    ->setFunctionField(ctx, setPosition);
  global->setField(ctx, "Camera", Camera);
}

core::AutoPtr<Value> Trait_Camera::create(core::AutoPtr<Script> ctx,
                                          core::AutoPtr<video::Camera> camera) {
  auto global = ctx->getNativeGlobal();
  auto Camera = global->getField(ctx, "Camera");
  auto instance = ctx->createValue()->setObject(ctx)->setOpaque(camera);
  for (auto &field : Camera->getKeys(ctx)) {
    instance->setField(ctx, field, Camera->getField(ctx, field));
  }
  return instance;
}