#include "script/helper/Trait_Animation.hpp"
#include "core/AutoPtr.hpp"
#include "document/Animation.hpp"
#include "script/Script.hpp"
#include "script/helper/Trait_Node.hpp"
using namespace firefly;
using namespace firefly::script;
using SelfType = core::AutoPtr<document::Animation>;
FUNC_DEF(Trait_Animation::setAction) {
  if (args.size() == 7) {
    auto [self, name, attr, step, startFrame, endFrame, loop] =
        Script::parseArgs<SelfType, std::string, std::string, float, uint32_t,
                          uint32_t, bool>(ctx, args);
    self->setAction(name, attr, step, startFrame, endFrame, loop);
  } else {
    auto [self, name, attr, startValue, finalValue, startFrame, endFrame,
          loop] = Script::parseArgs<SelfType, std::string, std::string, float,
                                    float, uint32_t, uint32_t, bool>(ctx, args);
    self->setAction(name, attr, startValue, finalValue, startFrame, endFrame,
                    loop);
  }
  return {};
}

FUNC_DEF(Trait_Animation::removeAction) {
  auto [self, name] = Script::parseArgs<SelfType, std::string>(ctx, args);
  self->removeAction(name);
  return {};
}

FUNC_DEF(Trait_Animation::setGroup) {
  auto [self, name] = Script::parseArgs<SelfType, std::string>(ctx, args);
  self->setGroup(name);
  return {};
}
FUNC_DEF(Trait_Animation::setSpeed) {
  auto [self, fps] = Script::parseArgs<SelfType, uint32_t>(ctx, args);
  self->setSpeed(fps);
  return {};
}
FUNC_DEF(Trait_Animation::start) {
  auto [self, name] = Script::parseArgs<SelfType, std::string>(ctx, args);
  self->start(name);
  return {};
}
FUNC_DEF(Trait_Animation::stop) {
  auto [self, name] = Script::parseArgs<SelfType, std::string>(ctx, args);
  self->stop(name);
  return {};
}
FUNC_DEF(Trait_Animation::resume) {
  auto [self, name] = Script::parseArgs<SelfType, std::string>(ctx, args);
  self->resume(name);
  return {};
}
FUNC_DEF(Trait_Animation::reset) {
  auto [self, name] = Script::parseArgs<SelfType, std::string>(ctx, args);
  self->reset(name);
  return {};
}
void Trait_Animation::initialize(core::AutoPtr<Script> ctx) {
  auto global = ctx->getNativeGlobal();
  auto Animation = ctx->createValue()
                       ->setObject(ctx)
                       ->setFunctionField(ctx, setAction)
                       ->setFunctionField(ctx, removeAction)
                       ->setFunctionField(ctx, setGroup)
                       ->setFunctionField(ctx, setSpeed)
                       ->setFunctionField(ctx, start)
                       ->setFunctionField(ctx, stop)
                       ->setFunctionField(ctx, reset)
                       ->setFunctionField(ctx, resume);
  global->setField(ctx, "Animation", Animation);
}

core::AutoPtr<Value>
Trait_Animation::create(core::AutoPtr<Script> ctx,
                        const core::AutoPtr<document::Animation> &animation) {
  auto global = ctx->getNativeGlobal();
  auto Animation = global->getField(ctx, "Animation");
  auto instance = Trait_Node::create(ctx, animation);
  for (auto &field : Animation->getKeys(ctx)) {
    instance->setField(ctx, field, Animation->getField(ctx, field));
  }
  return instance;
}