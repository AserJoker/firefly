#include "script/helper/Trait_Sprite2D.hpp"
#include "exception/ValidateException.hpp"
#include "script/Script.hpp"
#include "script/helper/Trait_Node.hpp"
#include "video/Sprite2D.hpp"
using namespace firefly;
using namespace firefly::script;
using exception::ValidateException;

FUNC_DEF(Trait_Sprite2D::setTexture) {
  VALIDATE_ARGS(setTexture, 2);
  auto self = args[0];
  auto path = args[1]->toString(ctx);
  auto sprite = self->getOpaque().cast<video::Sprite2D>();
  sprite->setTexture(path);
  return {};
}

FUNC_DEF(Trait_Sprite2D::getTextureSize) {
  VALIDATE_ARGS(setTexture, 1);
  auto self = args[0];
  auto sprite = self->getOpaque().cast<video::Sprite2D>();
  auto size = sprite->getTexture()->getSize();
  auto s = ctx->createValue()
               ->setObject(ctx)
               ->setField(ctx, "width", createNumber(ctx, size[0]))
               ->setField(ctx, "height", createNumber(ctx, size[1]));
  return {s};
}

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

FUNC_DEF(Trait_Sprite2D::getRect) {
  VALIDATE_ARGS(getRect, 1);
  auto self = args[0];
  auto sprite = self->getOpaque().cast<video::Sprite2D>();
  auto rc = sprite->getRect();
  auto rect = ctx->createValue()->setObject(ctx);
  rect->setField(ctx, "x", createNumber(ctx, rc[0]));
  rect->setField(ctx, "y", createNumber(ctx, rc[1]));
  rect->setField(ctx, "width", createNumber(ctx, rc[2]));
  rect->setField(ctx, "height", createNumber(ctx, rc[3]));
  return {rect};
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

FUNC_DEF(Trait_Sprite2D::getSourceRect) {
  VALIDATE_ARGS(getSourceRect, 1);
  auto self = args[0];
  auto sprite = self->getOpaque().cast<video::Sprite2D>();
  auto rc = sprite->getSourceRect();
  auto rect = ctx->createValue()->setObject(ctx);
  rect->setField(ctx, "x", createNumber(ctx, rc[0]));
  rect->setField(ctx, "y", createNumber(ctx, rc[1]));
  rect->setField(ctx, "width", createNumber(ctx, rc[2]));
  rect->setField(ctx, "height", createNumber(ctx, rc[3]));
  return {rect};
}

FUNC_DEF(Trait_Sprite2D::setRotation) {
  VALIDATE_ARGS(setRotation, 3);
  auto self = args[0];
  auto center = args[1];
  auto angle = args[2]->toNumber(ctx);
  auto x = (uint32_t)center->getField(ctx, "x")->toNumber(ctx);
  auto y = (uint32_t)center->getField(ctx, "y")->toNumber(ctx);
  auto sprite = self->getOpaque().cast<video::Sprite2D>();
  sprite->setRotation({x, y}, angle);
  return {};
}

FUNC_DEF(Trait_Sprite2D::getRotation) {
  VALIDATE_ARGS(getRotation, 1);
  auto self = args[0];
  auto sprite = self->getOpaque().cast<video::Sprite2D>();
  auto &[center, angle] = sprite->getRotation();
  auto rotation = ctx->createValue()->setObject(ctx);
  auto ct = ctx->createValue()->setObject(ctx);
  ct->setField(ctx, "x", createNumber(ctx, center[0]));
  ct->setField(ctx, "y", createNumber(ctx, center[1]));
  rotation->setField(ctx, "center", ct);
  rotation->setField(ctx, "angle", createNumber(ctx, angle));
  return {rotation};
}

FUNC_DEF(Trait_Sprite2D::setVisible) {
  VALIDATE_ARGS(setVisible, 2);
  auto self = args[0];
  auto sprite = self->getOpaque().cast<video::Sprite2D>();
  sprite->setVisible(args[1]->toBoolean(ctx));
  return {};
}

FUNC_DEF(Trait_Sprite2D::isVisible) {
  VALIDATE_ARGS(isVisible, 1);
  auto self = args[0];
  auto sprite = self->getOpaque().cast<video::Sprite2D>();
  return {createBoolean(ctx, sprite->isVisible())};
}

FUNC_DEF(Trait_Sprite2D::setZIndex) {
  VALIDATE_ARGS(setZIndex, 2);
  auto self = args[0];
  auto sprite = self->getOpaque().cast<video::Sprite2D>();
  sprite->setZIndex(args[1]->toNumber(ctx));
  return {};
}

FUNC_DEF(Trait_Sprite2D::getZIndex) {
  VALIDATE_ARGS(getZIndex, 1);
  auto self = args[0];
  auto sprite = self->getOpaque().cast<video::Sprite2D>();
  return {createNumber(ctx, sprite->getZIndex())};
}

FUNC_DEF(Trait_Sprite2D::setBlend) {
  VALIDATE_ARGS(setBlend, 2);
  auto self = args[0];
  auto sprite = self->getOpaque().cast<video::Sprite2D>();
  sprite->setBlend(args[1]->toNumber(ctx));
  return {};
}

FUNC_DEF(Trait_Sprite2D::getBlend) {
  VALIDATE_ARGS(getBlend, 1);
  auto self = args[0];
  auto sprite = self->getOpaque().cast<video::Sprite2D>();
  return {createNumber(ctx, sprite->getBlend())};
}

void Trait_Sprite2D::initialize(core::AutoPtr<Script> ctx) {
  auto global = ctx->getNativeGlobal();
  auto Sprite2D = ctx->createValue()
                      ->setObject(ctx)
                      ->setFunctionField(ctx, setTexture)
                      ->setFunctionField(ctx, getTextureSize)
                      ->setFunctionField(ctx, setRect)
                      ->setFunctionField(ctx, setSourceRect)
                      ->setFunctionField(ctx, setRotation)
                      ->setFunctionField(ctx, getRect)
                      ->setFunctionField(ctx, getSourceRect)
                      ->setFunctionField(ctx, getRotation)
                      ->setFunctionField(ctx, setVisible)
                      ->setFunctionField(ctx, isVisible)
                      ->setFunctionField(ctx, setZIndex)
                      ->setFunctionField(ctx, getZIndex)
                      ->setFunctionField(ctx, setBlend)
                      ->setFunctionField(ctx, getBlend);
  global->setField(ctx, "Sprite2D", Sprite2D);
}

core::AutoPtr<Value>
Trait_Sprite2D::create(core::AutoPtr<Script> ctx,
                       core::AutoPtr<video::Sprite2D> sprite) {
  auto global = ctx->getNativeGlobal();
  auto Sprite2D = global->getField(ctx, "Sprite2D");
  auto instance = Trait_Node::create(ctx, sprite);
  for (auto &field : Sprite2D->getKeys(ctx)) {
    instance->setField(ctx, field, Sprite2D->getField(ctx, field));
  }
  return instance;
}