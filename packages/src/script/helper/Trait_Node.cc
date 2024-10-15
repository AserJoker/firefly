#include "script/helper/Trait_Node.hpp"
#include "core/AutoPtr.hpp"
#include "core/Value.hpp"
#include "document/Node.hpp"
#include "script/Script.hpp"
#include "script/Value.hpp"

using namespace firefly;
using namespace firefly::script;
using SelfType = core::AutoPtr<document::Node>;
FUNC_DEF(Trait_Node::appendChild) {
  auto [self, child] = Script::parseArgs<SelfType, SelfType>(ctx, args);
  self->appendChild(child);
  return {};
}

FUNC_DEF(Trait_Node::removeChild) {
  auto [self, child] = Script::parseArgs<SelfType, SelfType>(ctx, args);

  self->removeChild(child);
  return {};
}
FUNC_DEF(Trait_Node::getParent) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  auto parent = self->getParent();
  if (!parent) {
    return {ctx->createValue()};
  }
  return {Trait_Node::create(ctx, self->getParent())};
}

FUNC_DEF(Trait_Node::getId) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  return {ctx->createValue(self->getId())};
}
FUNC_DEF(Trait_Node::setId) {
  auto [self, id] = Script::parseArgs<SelfType, std::string>(ctx, args);
  self->setId(id);
  return {};
}
FUNC_DEF(Trait_Node::getChild) {
  auto [self, id] = Script::parseArgs<SelfType, std::string>(ctx, args);
  auto child = self->getChild(id);
  return {Trait_Node::create(ctx, child)};
}
FUNC_DEF(Trait_Node::beginAttrGroup) {
  auto [self, key] = Script::parseArgs<SelfType, std::string>(ctx, args);
  self->beginAttrGroup(key);
  return {};
}
FUNC_DEF(Trait_Node::endAttrGroup) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  self->endAttrGroup();
  return {};
}
FUNC_DEF(Trait_Node::setAttribute) {
  auto [self, key] = Script::parseArgs<SelfType, std::string>(ctx, args);
  auto type = Atom::TYPE::NIL;
  if (args.size() > 2) {
    type = args[2]->getType(ctx);
  }
  auto res = false;
  switch (type) {
  case Atom::TYPE::NIL:
    res = self->setAttribute(key, core::Value{nullptr});
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
  auto [self, name, node, source] =
      Script::parseArgs<SelfType, std::string, SelfType, std::string>(ctx,
                                                                      args);
  self->bindAttribute(name, node, source);
  return {};
}

FUNC_DEF(Trait_Node::getAttribute) {
  auto [self, key] = Script::parseArgs<SelfType, std::string>(ctx, args);
  auto &attr = self->getAttribute(key);
  switch (attr.getType()) {
  case core::Value::Type::BOOLEAN:
    return {ctx->createValue(attr.toBoolean())};
  case core::Value::Type::STRING:
    return {ctx->createValue(attr.toString())};
  case core::Value::Type::I32:
    return {ctx->createValue(attr.toInt32())};
  case core::Value::Type::U32:
    return {ctx->createValue(attr.toUint32())};
  case core::Value::Type::F32:
    return {ctx->createValue(attr.toFloat32())};
  case core::Value::Type::NIL:
    return {};
  }
  return {};
}

FUNC_DEF(Trait_Node::getAttributes) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  auto attributes = self->getAttributes();
  auto result = ctx->createValue()->setObject(ctx);
  for (auto &[key, type] : attributes) {
    result->setField(ctx, key, ctx->createValue(type));
  }
  return {result};
}
FUNC_DEF(Trait_Node::getAttributeGroups) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  auto groups = self->getAttributeGroups();
  auto result = ctx->createValue()->setObject(ctx);
  for (auto &[name, attributes] : groups) {
    auto attrs = ctx->createValue()->setArray(ctx);
    for (size_t index = 0; index < attributes.size(); index++) {
      attrs->setIndex(ctx, index, ctx->createValue(attributes[index]));
    }
    result->setField(ctx, name, attrs);
  }
  return {result};
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
                  ->setFunctionField(ctx, endAttrGroup)
                  ->setFunctionField(ctx, getAttributes)
                  ->setFunctionField(ctx, getAttributeGroups);
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