#include "script/helper/Trait_Sprite2D.hpp"
#include "exception/ValidateException.hpp"
#include "video/Sprite2D.hpp"
using namespace firefly;
using namespace firefly::script;
using exception::ValidateException;
FUNC_DEF(Trait_Sprite2D::setRect) {
  VALIDATE_ARGS(setRect, 2);
  auto self = args[0];
  auto rect = args[1];
  auto x = (uint32_t)rect->getField(ctx, "x")->toNumber(ctx);
  auto y = (uint32_t)rect->getField(ctx, "y")->toNumber(ctx);
  auto width = (uint32_t)rect->getField(ctx, "width")->toNumber(ctx);
  auto height = (uint32_t)rect->getField(ctx, "height")->toNumber(ctx);
  auto sprite = self->getOpaque().cast<video::Sprite2D>();
  sprite->setRect({x, y, width, height});
  return {};
}

FUNC_DEF(Trait_Sprite2D::setSourceRect) {
  VALIDATE_ARGS(setSourceRect, 2);
  auto self = args[0];
  auto rect = args[1];
  auto x = (uint32_t)rect->getField(ctx, "x")->toNumber(ctx);
  auto y = (uint32_t)rect->getField(ctx, "y")->toNumber(ctx);
  auto width = (uint32_t)rect->getField(ctx, "width")->toNumber(ctx);
  auto height = (uint32_t)rect->getField(ctx, "height")->toNumber(ctx);
  auto sprite = self->getOpaque().cast<video::Sprite2D>();
  sprite->setSourceRect({x, y, width, height});
  return {};
}

FUNC_DEF(Trait_Sprite2D::setTexture) {
  VALIDATE_ARGS(setTexture, 2);
  auto self = args[0];
  auto path = args[1]->toString(ctx);
  auto sprite = self->getOpaque().cast<video::Sprite2D>();
  sprite->setTexture(path);
  return {};
}
FUNC_DEF(Trait_Sprite2D::setRotation) {
  VALIDATE_ARGS(setTexture, 3);
  auto self = args[0];
  auto center = args[1];
  auto angle = args[2]->toNumber(ctx);
  bool righthandle = args.size() > 3 ? args[3]->toBoolean(ctx) : false;
  auto x = (uint32_t)center->getField(ctx, "x")->toNumber(ctx);
  auto y = (uint32_t)center->getField(ctx, "y")->toNumber(ctx);
  auto sprite = self->getOpaque().cast<video::Sprite2D>();
  sprite->setRotation({x, y}, angle, righthandle);
  return {};
}

void Trait_Sprite2D::initialize(core::AutoPtr<Script> ctx) {
  auto global = ctx->getNativeGlobal();
  auto Sprite2D = ctx->createValue()
                      ->setObject(ctx)
                      ->setFunctionField(ctx, setRect)
                      ->setFunctionField(ctx, setSourceRect)
                      ->setFunctionField(ctx, setTexture)
                      ->setFunctionField(ctx, setRotation);
  global->setField(ctx, "Sprite2D", Sprite2D);
}

core::AutoPtr<Value>
Trait_Sprite2D::create(core::AutoPtr<Script> ctx,
                       core::AutoPtr<video::Sprite2D> sprite) {
  auto global = ctx->getNativeGlobal();
  auto Buffer = global->getField(ctx, "Sprite2D");
  auto instance = ctx->createValue()->setObject(ctx)->setOpaque(sprite);
  for (auto &field : Buffer->getKeys(ctx)) {
    instance->setField(ctx, field, Buffer->getField(ctx, field));
  }
  return instance;
}