#include "script/helper/Trait_Node.hpp"
#include "core/AutoPtr.hpp"
#include "script/Script.hpp"
#include "script/Value.hpp"
#include "video/Node.hpp"
using namespace firefly;
using namespace firefly::script;

FUNC_DEF(Trait_Node::appendChild) {
  auto self = args[0]->getOpaque().cast<video::Node>();
  auto child = args[1]->getOpaque().cast<video::Node>();
  self->appendChild(child);
  return {};
}

FUNC_DEF(Trait_Node::removeChild) {
  auto self = args[0]->getOpaque().cast<video::Node>();
  auto child = args[1]->getOpaque().cast<video::Node>();
  self->removeChild(child);
  return {};
}
FUNC_DEF(Trait_Node::getParent) {
  auto self = args[0]->getOpaque().cast<video::Node>();
  auto parent = self->getParent();
  if (!parent) {
    return {ctx->createValue()};
  }
  return {Trait_Node::create(ctx, self->getParent())};
}

void Trait_Node::initialize(core::AutoPtr<Script> ctx) {
  auto global = ctx->getNativeGlobal();
  auto Node = ctx->createValue()
                  ->setObject(ctx)
                  ->setFunctionField(ctx, appendChild)
                  ->setFunctionField(ctx, removeChild)
                  ->setFunctionField(ctx, getParent);
  global->setField(ctx, "Node", Node);
}

core::AutoPtr<Value>
Trait_Node::create(core::AutoPtr<Script> ctx,
                   const core::AutoPtr<video::Node> &node) {
  auto global = ctx->getNativeGlobal();
  auto Node = global->getField(ctx, "Node");
  auto instance = ctx->createValue()->setObject(ctx)->setOpaque(node);
  for (auto &field : Node->getKeys(ctx)) {
    instance->setField(ctx, field, Node->getField(ctx, field));
  }
  return instance;
}