#include "script/lib/Module_Document.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "document/Animation.hpp"
#include "document/ParticleGenerator.hpp"
#include "document/RenderTarget.hpp"
#include "document/Scene.hpp"
#include "document/Sprite2D.hpp"
#include "exception/ValidateException.hpp"
#include "script/Atom.hpp"
#include "script/Script.hpp"
#include "script/helper/Trait_Animation.hpp"
#include "script/helper/Trait_ParticleGenerator.hpp"
#include "script/helper/Trait_RenderTarget.hpp"
#include "script/helper/Trait_Scene.hpp"
#include "script/helper/Trait_Sprite2D.hpp"
#include "video/Renderer.hpp"

using namespace firefly;
using namespace firefly::script;
using exception::ValidateException;

FUNC_DEF(Module_Document::createSprite2D) {

  core::AutoPtr<document::Sprite2D> sprite;
  if (args[0]->getType(ctx) == Atom::TYPE::STRING) {
    auto path = args[0]->toString(ctx);
    sprite = new document::Sprite2D(path);
  } else {
    auto renderTarget = args[0]->getOpaque().cast<document::RenderTarget>();
    sprite = new document::Sprite2D();
    auto texture = renderTarget->getRenderTarget()->getAttachments()[0];
    auto size = texture->getSize();
    sprite->setTexture(texture);
    sprite->setRect({0, 0, size});
    sprite->setSourceRect({0, 0, size});
  }
  return {Trait_Sprite2D::create(ctx, sprite)};
}

FUNC_DEF(Module_Document::createScene) {
  core::AutoPtr scene = new document::Scene();
  return {Trait_Scene::create(ctx, scene)};
}

FUNC_DEF(Module_Document::createRenderTarget) {
  auto width = (uint32_t)args[0]->getField(ctx, "width")->toNumber(ctx);
  auto height = (uint32_t)args[0]->getField(ctx, "height")->toNumber(ctx);
  core::AutoPtr renderTarget = new document::RenderTarget({width, height});
  return {Trait_RenderTarget::create(ctx, renderTarget)};
}
FUNC_DEF(Module_Document::createAnimation) {
  core::AutoPtr animation = new document::Animation();
  return {Trait_Animation::create(ctx, animation)};
}

FUNC_DEF(Module_Document::createParticleGenerator) {
  auto [count] = Script::parseArgs<uint32_t>(ctx, args);
  core::AutoPtr generator = new document::ParticleGenerator(count);
  return {Trait_ParticleGenerator::create(ctx, generator)};
}

FUNC_DEF(Module_Document::setScene) {
  auto scene = args[0]->getOpaque().cast<document::Scene>();
  document::Scene::scene = scene;
  ctx->getNativeGlobal()->setField(ctx, "$scene", args[0]);
  return {};
}

FUNC_DEF(Module_Document::getScene) {
  auto obj = ctx->getNativeGlobal()->getField(ctx, "$scene");
  return {obj};
}

FUNC_DEF(Module_Document::setShader) {
  auto [name] = Script::parseArgs<std::string>(ctx, args);
  auto renderer = core::Singleton<video::Renderer>::instance();
  renderer->setShader(name);
  return {};
}

void Module_Document::open(core::AutoPtr<Script> ctx) {
  auto exports = ctx->createValue()->setObject(ctx);

  exports->setFunctionField(ctx, createSprite2D)
      ->setFunctionField(ctx, createScene)
      ->setFunctionField(ctx, createRenderTarget)
      ->setFunctionField(ctx, createAnimation)
      ->setFunctionField(ctx, createParticleGenerator)
      ->setFunctionField(ctx, setShader)
      ->setFunctionField(ctx, setScene)
      ->setFunctionField(ctx, getScene);

  ctx->registerModule("document", exports);
}