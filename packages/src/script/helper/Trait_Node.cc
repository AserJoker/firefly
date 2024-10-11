#include "script/helper/Trait_Node.hpp"
#include "core/Attribute.hpp"
#include "core/AutoPtr.hpp"
#include "document/Node.hpp"
#include "script/Script.hpp"
#include "script/Value.hpp"

using namespace firefly;
using namespace firefly::script;

FUNC_DEF(Trait_Node::appendChild) {
  auto self = args[0]->getOpaque().cast<document::Node>();
  auto child = args[1]->getOpaque().cast<document::Node>();
  self->appendChild(child);
  return {};
}

FUNC_DEF(Trait_Node::removeChild) {
  auto self = args[0]->getOpaque().cast<document::Node>();
  auto child = args[1]->getOpaque().cast<document::Node>();
  self->removeChild(child);
  return {};
}
FUNC_DEF(Trait_Node::getParent) {
  auto self = args[0]->getOpaque().cast<document::Node>();
  auto parent = self->getParent();
  if (!parent) {
    return {ctx->createValue()};
  }
  return {Trait_Node::create(ctx, self->getParent())};
}

FUNC_DEF(Trait_Node::getId) {
  auto self = args[0]->getOpaque().cast<document::Node>();
  return {ctx->createValue()->setString(ctx, self->getId())};
}
FUNC_DEF(Trait_Node::setId) {
  auto self = args[0]->getOpaque().cast<document::Node>();
  auto id = args[1]->toString(ctx);
  self->setId(id);
  return {};
}
FUNC_DEF(Trait_Node::getChild) {
  auto self = args[0]->getOpaque().cast<document::Node>();
  auto id = args[1]->toString(ctx);
  auto child = self->getChild(id);
  return {Trait_Node::create(ctx, child)};
}
FUNC_DEF(Trait_Node::beginAttrGroup) {
  auto self = args[0]->getOpaque().cast<document::Node>();
  auto key = args[1]->toString(ctx);
  self->beginAttrGroup(key);
  return {};
}
FUNC_DEF(Trait_Node::endAttrGroup) {
  auto self = args[0]->getOpaque().cast<document::Node>();
  self->endAttrGroup();
  return {};
}
FUNC_DEF(Trait_Node::setAttribute) {
  auto self = args[0]->getOpaque().cast<document::Node>();
  auto key = args[1]->toString(ctx);
  auto type = Atom::TYPE::NIL;
  if (args.size() > 2) {
    type = args[2]->getType(ctx);
  }
  auto res = false;
  switch (type) {
  case Atom::TYPE::NIL:
    res = self->setAttribute(key, core::Attribute{nullptr});
    break;
  case Atom::TYPE::NUMBER:
    res = self->setAttribute(key, float(args[2]->toNumber(ctx)));
    break;
  case Atom::TYPE::BOOLEAN:
    res = self->setAttribute(key, args[2]->toBoolean(ctx));
    break;
  case Atom::TYPE::STRING:
    res = self->setAttribute(key, args[2]->toString(ctx));
    break;
  default:
    break;
  }
  return {ctx->createValue(res)};
}
FUNC_DEF(Trait_Node::bindAttribute) {
  auto self = args[0]->getOpaque().cast<document::Node>();
  auto name = args[1]->toString(ctx);
  auto node = args[2]->getOpaque().cast<document::Node>();
  auto source = args[3]->toString(ctx);
  self->bindAttribute(name, node, source);
  return {};
}

FUNC_DEF(Trait_Node::getAttribute) {
  auto self = args[0]->getOpaque().cast<document::Node>();
  auto key = args[1]->toString(ctx);
  auto &attr = self->getAttribute(key);
  switch (attr.getType()) {
  case core::AttributeType::BOOLEAN:
    return {ctx->createValue(attr.toBoolean())};
  case core::AttributeType::STRING:
    return {ctx->createValue(attr.toString())};
  case core::AttributeType::I32:
    return {ctx->createValue(attr.toInt32())};
  case core::AttributeType::U32:
    return {ctx->createValue(attr.toUint32())};
  case core::AttributeType::F32:
    return {ctx->createValue(attr.toFloat32())};
  case core::AttributeType::NIL:
    return {};
  }
  return {};
}

void Trait_Node::initialize(core::AutoPtr<Script> ctx) {
  auto global = ctx->getNativeGlobal();
  auto Node = ctx->createValue()
                  ->setObject(ctx)
                  ->setFunctionField(ctx, appendChild)
                  ->setFunctionField(ctx, removeChild)
                  ->setFunctionField(ctx, getParent)
                  ->setFunctionField(ctx, getId)
                  ->setFunctionField(ctx, setId)
                  ->setFunctionField(ctx, getChild)
                  ->setFunctionField(ctx, getAttribute)
                  ->setFunctionField(ctx, setAttribute)
                  ->setFunctionField(ctx, bindAttribute)
                  ->setFunctionField(ctx, beginAttrGroup)
                  ->setFunctionField(ctx, endAttrGroup);
  global->setField(ctx, "Node", Node);
}

core::AutoPtr<Value>
Trait_Node::create(core::AutoPtr<Script> ctx,
                   const core::AutoPtr<document::Node> &node) {
  auto global = ctx->getNativeGlobal();
  auto Node = global->getField(ctx, "Node");
  auto instance = ctx->createValue()->setObject(ctx)->setOpaque(node);
  for (auto &field : Node->getKeys(ctx)) {
    instance->setField(ctx, field, Node->getField(ctx, field));
  }
  return instance;
}