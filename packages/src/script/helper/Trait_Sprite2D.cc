#include "script/helper/Trait_Sprite2D.hpp"
#include "core/AutoPtr.hpp"
#include "document/Sprite2D.hpp"
#include "exception/ValidateException.hpp"
#include "script/Script.hpp"
#include "script/helper/Trait_Node.hpp"

using namespace firefly;
using namespace firefly::script;
using exception::ValidateException;
using SelfType = core::AutoPtr<document::Sprite2D>;
FUNC_DEF(Trait_Sprite2D::setTexture) {
  auto [self, path] = Script::parseArgs<SelfType, std::string>(ctx, args);
  self->setTexture(path);
  return {};
}

FUNC_DEF(Trait_Sprite2D::getTextureSize) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  auto size = self->getTextureObject()->getSize();
  auto s = ctx->createValue()
               ->setObject(ctx)
               ->setField(ctx, "width", ctx->createValue(size[0]))
               ->setField(ctx, "height", ctx->createValue(size[1]));
  return {s};
}

FUNC_DEF(Trait_Sprite2D::setRect) {
  auto [self, rect] =
      Script::parseArgs<SelfType, core::AutoPtr<Value>>(ctx, args);
  auto x = (uint32_t)rect->getField(ctx, "x")->toNumber(ctx);
  auto y = (uint32_t)rect->getField(ctx, "y")->toNumber(ctx);
  auto width = (uint32_t)rect->getField(ctx, "width")->toNumber(ctx);
  auto height = (uint32_t)rect->getField(ctx, "height")->toNumber(ctx);
  self->setRect({x, y, width, height});
  return {};
}

FUNC_DEF(Trait_Sprite2D::getRect) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  auto rc = self->getRect();
  auto rect = ctx->createValue()->setObject(ctx);
  rect->setField(ctx, "x", ctx->createValue(rc[0]));
  rect->setField(ctx, "y", ctx->createValue(rc[1]));
  rect->setField(ctx, "width", ctx->createValue(rc[2]));
  rect->setField(ctx, "height", ctx->createValue(rc[3]));
  return {rect};
}

FUNC_DEF(Trait_Sprite2D::setSourceRect) {
  auto [self, rect] =
      Script::parseArgs<SelfType, core::AutoPtr<Value>>(ctx, args);
  auto x = (uint32_t)rect->getField(ctx, "x")->toNumber(ctx);
  auto y = (uint32_t)rect->getField(ctx, "y")->toNumber(ctx);
  auto width = (uint32_t)rect->getField(ctx, "width")->toNumber(ctx);
  auto height = (uint32_t)rect->getField(ctx, "height")->toNumber(ctx);
  self->setSourceRect({x, y, width, height});
  return {};
}

FUNC_DEF(Trait_Sprite2D::getSourceRect) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  auto rc = self->getSourceRect();
  auto rect = ctx->createValue()->setObject(ctx);
  rect->setField(ctx, "x", ctx->createValue(rc[0]));
  rect->setField(ctx, "y", ctx->createValue(rc[1]));
  rect->setField(ctx, "width", ctx->createValue(rc[2]));
  rect->setField(ctx, "height", ctx->createValue(rc[3]));
  return {rect};
}

FUNC_DEF(Trait_Sprite2D::setRotation) {
  auto [self, center, angle] =
      Script::parseArgs<SelfType, core::AutoPtr<Value>, float>(ctx, args);
  auto x = (uint32_t)center->getField(ctx, "x")->toNumber(ctx);
  auto y = (uint32_t)center->getField(ctx, "y")->toNumber(ctx);
  self->setRotation({x, y}, angle);
  return {};
}

FUNC_DEF(Trait_Sprite2D::getRotation) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  auto &[center, angle] = self->getRotation();
  auto rotation = ctx->createValue()->setObject(ctx);
  auto ct = ctx->createValue()->setObject(ctx);
  ct->setField(ctx, "x", ctx->createValue(center[0]));
  ct->setField(ctx, "y", ctx->createValue(center[1]));
  rotation->setField(ctx, "center", ct);
  rotation->setField(ctx, "angle", ctx->createValue(angle));
  return {rotation};
}

FUNC_DEF(Trait_Sprite2D::setVisible) {
  auto [self, visible] = Script::parseArgs<SelfType, bool>(ctx, args);
  self->setVisible(visible);
  return {};
}

FUNC_DEF(Trait_Sprite2D::isVisible) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  return {ctx->createValue(self->isVisible())};
}

FUNC_DEF(Trait_Sprite2D::setZIndex) {
  auto [self, zIndex] = Script::parseArgs<SelfType, int32_t>(ctx, args);
  self->setZIndex(zIndex);
  return {};
}

FUNC_DEF(Trait_Sprite2D::getZIndex) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  return {ctx->createValue(self->getZIndex())};
}

FUNC_DEF(Trait_Sprite2D::setBlend) {
  auto [self, blend] = Script::parseArgs<SelfType, int32_t>(ctx, args);
  self->setBlend(blend);
  return {};
}

FUNC_DEF(Trait_Sprite2D::getBlend) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  return {ctx->createValue(self->getBlend())};
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
                       core::AutoPtr<document::Sprite2D> sprite) {
  auto global = ctx->getNativeGlobal();
  auto Sprite2D = global->getField(ctx, "Sprite2D");
  auto instance = Trait_Node::create(ctx, sprite);
  for (auto &field : Sprite2D->getKeys(ctx)) {
    instance->setField(ctx, field, Sprite2D->getField(ctx, field));
  }
  return instance;
}