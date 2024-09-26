#include "script/lib/Module_Video.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "exception/ValidateException.hpp"
#include "script/Atom.hpp"
#include "script/Script.hpp"
#include "script/helper/Trait_RenderTarget.hpp"
#include "script/helper/Trait_Scene.hpp"
#include "script/helper/Trait_Sprite2D.hpp"
#include "video/RenderTarget.hpp"
#include "video/Renderer.hpp"
#include "video/Scene.hpp"
#include "video/Sprite2D.hpp"
using namespace firefly;
using namespace firefly::script;
using exception::ValidateException;

FUNC_DEF(Module_Video::createSprite2D) {
  VALIDATE_ARGS(createSprite2D, 1);
  core::AutoPtr<video::Sprite2D> sprite;
  if (args[0]->getType(ctx) == Atom::TYPE::STRING) {
    auto path = args[0]->toString(ctx);
    sprite = new video::Sprite2D(path);
  } else {
    auto renderTarget = args[0]->getOpaque().cast<video::RenderTarget>();
    sprite = new video::Sprite2D();
    auto texture = renderTarget->getAttachments()[0];
    auto size = texture->getSize();
    sprite->setTexture(texture);
    sprite->setRect({0, 0, size});
    sprite->setSourceRect({0, 0, size});
  }
  return {Trait_Sprite2D::create(ctx, sprite)};
}

FUNC_DEF(Module_Video::createScene) {
  core::AutoPtr scene = new video::Scene();
  return {Trait_Scene::create(ctx, scene)};
}

FUNC_DEF(Module_Video::createRenderTarget) {
  auto width = (uint32_t)args[0]->getField(ctx, "width")->toNumber(ctx);
  auto height = (uint32_t)args[0]->getField(ctx, "height")->toNumber(ctx);
  core::AutoPtr renderTarget = new video::RenderTarget({width, height});
  return {Trait_RenderTarget::create(ctx, renderTarget)};
}

FUNC_DEF(Module_Video::setScene) {
  auto scene = args[0]->getOpaque().cast<video::Scene>();
  video::Scene::scene = scene;
  ctx->getNativeGlobal()->setField(ctx, "$scene", args[0]);
  return {};
}

FUNC_DEF(Module_Video::getScene) {
  auto obj = ctx->getNativeGlobal()->getField(ctx, "$scene");
  return {obj};
}

FUNC_DEF(Module_Video::setShader) {
  VALIDATE_ARGS(setShader, 1);
  auto name = args[0]->toString(ctx);
  auto renderer = core::Singleton<video::Renderer>::instance();
  renderer->setShader(name);
  return {};
}

void Module_Video::open(core::AutoPtr<Script> ctx) {
  auto exports = ctx->createValue()->setObject(ctx);

  exports->setFunctionField(ctx, createSprite2D)
      ->setFunctionField(ctx, createScene)
      ->setFunctionField(ctx, createRenderTarget)
      ->setFunctionField(ctx, setShader)
      ->setFunctionField(ctx, setScene)
      ->setFunctionField(ctx, getScene);

  ctx->registerModule("video", exports);
}