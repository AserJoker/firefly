#include "script/helper/Trait_Scene.hpp"
#include "core/AutoPtr.hpp"
#include "document/Scene.hpp"
#include "exception/Exception.hpp"
#include "script/Script.hpp"
#include "script/Value.hpp"
#include "script/helper/Trait_Node.hpp"
using namespace firefly;
using namespace firefly::script;
using SelfType = core::AutoPtr<document::Scene>;
FUNC_DEF(Trait_Scene::setCamera) {
  auto [self, type] = Script::parseArgs<SelfType, std::string>(ctx, args);
  if (type == "ortho") {
    self->setCamera(document::Scene::CameraType::ORTHO);
  } else if (type == "perspective") {
    self->setCamera(document::Scene::CameraType::PERSPECTIVE);
  } else {
    throw exception::RuntimeException<"LuaException">(
        fmt::format("Unknown camera type:'{}'", type));
  }
  return {};
}

void Trait_Scene::initialize(core::AutoPtr<Script> ctx) {
  auto global = ctx->getNativeGlobal();
  auto Scene =
      ctx->createValue()->setObject(ctx)->setFunctionField(ctx, setCamera);
  global->setField(ctx, "Scene", Scene);
}

core::AutoPtr<Value>
Trait_Scene::create(core::AutoPtr<Script> ctx,
                    const core::AutoPtr<document::Scene> &scene) {
  auto global = ctx->getNativeGlobal();
  auto Scene = global->getField(ctx, "Scene");
  auto instance = Trait_Node::create(ctx, scene);
  for (auto &field : Scene->getKeys(ctx)) {
    instance->setField(ctx, field, Scene->getField(ctx, field));
  }
  return instance;
}