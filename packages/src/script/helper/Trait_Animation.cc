#include "script/helper/Trait_Animation.hpp"
#include "script/helper/Trait_Node.hpp"
using namespace firefly;
using namespace firefly::script;

FUNC_DEF(Trait_Animation::setAction) {
  auto self = args[0]->getOpaque().cast<document::Animation>();
  if (args.size() == 7) {
    auto name = args[1]->toString(ctx);
    auto attr = args[2]->toString(ctx);
    auto step = args[3]->toNumber(ctx);
    auto startFrame = (uint32_t)args[4]->toNumber(ctx);
    auto endFrame = (uint32_t)args[5]->toNumber(ctx);
    auto loop = args[6]->toBoolean(ctx);
    self->setAction(name, attr, step, startFrame, endFrame, loop);
  } else {
    auto name = args[1]->toString(ctx);
    auto attr = args[2]->toString(ctx);
    auto startValue = args[3]->toNumber(ctx);
    auto finalValue = args[4]->toNumber(ctx);
    auto startFrame = (uint32_t)args[5]->toNumber(ctx);
    auto endFrame = (uint32_t)args[6]->toNumber(ctx);
    auto loop = args[7]->toBoolean(ctx);
    self->setAction(name, attr, startValue, finalValue, startFrame, endFrame,
                    loop);
  }
  return {};
}

FUNC_DEF(Trait_Animation::removeAction) {
  auto self = args[0]->getOpaque().cast<document::Animation>();
  auto name = args[1]->toString(ctx);
  self->removeAction(name);
  return {};
}

FUNC_DEF(Trait_Animation::setGroup) {
  auto self = args[0]->getOpaque().cast<document::Animation>();
  auto name = args[1]->toString(ctx);
  self->setGroup(name);
  return {};
}
FUNC_DEF(Trait_Animation::setFPS) {
  auto self = args[0]->getOpaque().cast<document::Animation>();
  auto fps = (uint32_t)args[1]->toNumber(ctx);
  self->setSpeed(fps);
  return {};
}
FUNC_DEF(Trait_Animation::start) {
  auto self = args[0]->getOpaque().cast<document::Animation>();
  auto name = args[1]->toString(ctx);
  self->start(name);
  return {};
}
FUNC_DEF(Trait_Animation::stop) {
  auto self = args[0]->getOpaque().cast<document::Animation>();
  auto name = args[1]->toString(ctx);
  self->stop(name);
  return {};
}
FUNC_DEF(Trait_Animation::resume) {
  auto self = args[0]->getOpaque().cast<document::Animation>();
  auto name = args[1]->toString(ctx);
  self->resume(name);
  return {};
}
FUNC_DEF(Trait_Animation::reset) {
  auto self = args[0]->getOpaque().cast<document::Animation>();
  auto name = args[1]->toString(ctx);
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
                       ->setFunctionField(ctx, setFPS)
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