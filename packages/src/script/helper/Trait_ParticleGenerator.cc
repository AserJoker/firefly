#include "script/helper/Trait_ParticleGenerator.hpp"
#include "core/AutoPtr.hpp"
#include "document/ParticleGenerator.hpp"
#include "script/Script.hpp"
#include "script/helper/Trait_Node.hpp"
#include <unordered_map>
using namespace firefly;
using namespace firefly::script;
using SelfType = core::AutoPtr<document::ParticleGenerator>;
FUNC_DEF(Trait_ParticleGenerator::setTexture) {
  auto [self, texture] = Script::parseArgs<SelfType, std::string>(ctx, args);
  self->setTexture(texture);
  return {};
}
FUNC_DEF(Trait_ParticleGenerator::getTexture) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  return {ctx->createValue(self->getTexture())};
}
FUNC_DEF(Trait_ParticleGenerator::setDirection) {
  auto [self, direction] =
      Script::parseArgs<SelfType, core::AutoPtr<Value>>(ctx, args);
  auto x = (int32_t)direction->getField(ctx, "x")->toNumber(ctx);
  auto y = (int32_t)direction->getField(ctx, "y")->toNumber(ctx);
  self->setDirection({x, y});
  return {};
}
FUNC_DEF(Trait_ParticleGenerator::getDirection) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  auto direction = self->getDirection();
  return {ctx->createValue()
              ->setObject(ctx)
              ->setField(ctx, "x", ctx->createValue(direction.x))
              ->setField(ctx, "y", ctx->createValue(direction.y))};
}
FUNC_DEF(Trait_ParticleGenerator::setSpeed) {
  auto [self, speed] = Script::parseArgs<SelfType, float>(ctx, args);
  self->setSpeed(speed);
  return {};
}
FUNC_DEF(Trait_ParticleGenerator::getSpeed) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  return {ctx->createValue(self->getSpeed())};
}
FUNC_DEF(Trait_ParticleGenerator::setLinearAcceleration) {
  auto [self, acc] = Script::parseArgs<SelfType, float>(ctx, args);
  self->setLinearAcceleration(acc);
  return {};
}
FUNC_DEF(Trait_ParticleGenerator::getLinearAcceleration) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  return {ctx->createValue(self->getLinearAcceleration())};
}
FUNC_DEF(Trait_ParticleGenerator::setTangentialAcceleration) {
  auto [self, acc] = Script::parseArgs<SelfType, float>(ctx, args);
  self->setTangentialAcceleration(acc);
  return {};
}
FUNC_DEF(Trait_ParticleGenerator::getTangentialAcceleration) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  return {ctx->createValue(self->getTangentialAcceleration())};
}
FUNC_DEF(Trait_ParticleGenerator::setRadialAcceleration) {
  auto [self, acc] = Script::parseArgs<SelfType, float>(ctx, args);
  self->setRadialAcceleration(acc);
  return {};
}
FUNC_DEF(Trait_ParticleGenerator::getRadialAcceleration) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  return {ctx->createValue(self->getRadialAcceleration())};
}
FUNC_DEF(Trait_ParticleGenerator::setSize) {
  auto [self, size] =
      Script::parseArgs<SelfType, core::AutoPtr<Value>>(ctx, args);
  auto width = (float)size->getField(ctx, "width")->toNumber(ctx);
  auto height = (float)size->getField(ctx, "height")->toNumber(ctx);
  self->setSize({width, height});
  return {};
}
FUNC_DEF(Trait_ParticleGenerator::getSize) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  auto &size = self->getSize();
  return {ctx->createValue(std::unordered_map<std::string, AnyValue>{
      {"width", size.x}, {"height", size.y}})};
}
FUNC_DEF(Trait_ParticleGenerator::setPosition) {
  auto [self, position] =
      Script::parseArgs<SelfType, core::AutoPtr<Value>>(ctx, args);
  auto x = (float)position->getField(ctx, "x")->toNumber(ctx);
  auto y = (float)position->getField(ctx, "y")->toNumber(ctx);
  self->setPosition({x, y});
  return {};
}
FUNC_DEF(Trait_ParticleGenerator::getPosition) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  auto pos = self->getPosition();
  return {ctx->createValue(
      std::unordered_map<std::string, AnyValue>{{"x", pos.x}, {"y", pos.y}})};
}
FUNC_DEF(Trait_ParticleGenerator::setDelay) {
  auto [self, delay] = Script::parseArgs<SelfType, uint32_t>(ctx, args);
  self->setDelay(delay);
  return {};
}
FUNC_DEF(Trait_ParticleGenerator::getDelay) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  return {ctx->createValue(self->getDelay())};
}
FUNC_DEF(Trait_ParticleGenerator::setRandomAngle) {
  auto [self, angle] = Script::parseArgs<SelfType, uint32_t>(ctx, args);
  self->setRandomAngle(angle);
  return {};
}
FUNC_DEF(Trait_ParticleGenerator::getRandomAngle) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  return {ctx->createValue(self->getRandomAngle())};
}
FUNC_DEF(Trait_ParticleGenerator::setGeneratorCount) {
  auto [self, count] = Script::parseArgs<SelfType, uint32_t>(ctx, args);
  self->setGeneratorCount(count);
  return {};
}
FUNC_DEF(Trait_ParticleGenerator::getGeneratorCount) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  return {ctx->createValue(self->getGeneratorCount())};
}
FUNC_DEF(Trait_ParticleGenerator::setLocalCoord) {
  auto [self, local] = Script::parseArgs<SelfType, bool>(ctx, args);
  self->setLocalCoord(local);
  return {};
}
FUNC_DEF(Trait_ParticleGenerator::isLocalCoord) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  return {ctx->createValue(self->isLocalCoord())};
}
FUNC_DEF(Trait_ParticleGenerator::setColor) {
  auto [self, color] =
      Script::parseArgs<SelfType, core::AutoPtr<Value>>(ctx, args);
  auto r = (float)color->getField(ctx, "r")->toNumber(ctx);
  auto g = (float)color->getField(ctx, "g")->toNumber(ctx);
  auto b = (float)color->getField(ctx, "b")->toNumber(ctx);
  self->setColor({r, g, b});
  return {};
}
FUNC_DEF(Trait_ParticleGenerator::getColor) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  auto color = self->getColor();
  return {ctx->createValue(std::unordered_map<std::string, AnyValue>{
      {"r", color.r}, {"g", color.g}, {"b", color.b}})};
}
FUNC_DEF(Trait_ParticleGenerator::setLifetime) {
  auto [self, lifetime] = Script::parseArgs<SelfType, float>(ctx, args);
  self->setLifetime(lifetime);
  return {};
}
FUNC_DEF(Trait_ParticleGenerator::getLifetime) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  return {ctx->createValue(self->getLifetime())};
}
FUNC_DEF(Trait_ParticleGenerator::setShader) {
  auto [self, shader] = Script::parseArgs<SelfType, std::string>(ctx, args);
  self->setShader(shader);
  return {};
}
FUNC_DEF(Trait_ParticleGenerator::getShader) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  return {ctx->createValue(self->getShader())};
}
FUNC_DEF(Trait_ParticleGenerator::generateParticle) {
  auto [self, count] = Script::parseArgs<SelfType, uint32_t>(ctx, args);
  self->generateParticle(count);
  return {};
}
void Trait_ParticleGenerator::initialize(core::AutoPtr<Script> ctx) {
  auto global = ctx->getNativeGlobal();
  auto ParticleGenerator = ctx->createValue();
  global->setField(ctx, "ParticleGenerator", ParticleGenerator)
      ->setFunctionField(ctx, setTexture)
      ->setFunctionField(ctx, getTexture)
      ->setFunctionField(ctx, setDirection)
      ->setFunctionField(ctx, getDirection)
      ->setFunctionField(ctx, setSpeed)
      ->setFunctionField(ctx, getSpeed)
      ->setFunctionField(ctx, setLinearAcceleration)
      ->setFunctionField(ctx, getLinearAcceleration)
      ->setFunctionField(ctx, setTangentialAcceleration)
      ->setFunctionField(ctx, getTangentialAcceleration)
      ->setFunctionField(ctx, setRadialAcceleration)
      ->setFunctionField(ctx, getRadialAcceleration)
      ->setFunctionField(ctx, setSize)
      ->setFunctionField(ctx, getSize)
      ->setFunctionField(ctx, setPosition)
      ->setFunctionField(ctx, getPosition)
      ->setFunctionField(ctx, setDelay)
      ->setFunctionField(ctx, getDelay)
      ->setFunctionField(ctx, setRandomAngle)
      ->setFunctionField(ctx, getRandomAngle)
      ->setFunctionField(ctx, setGeneratorCount)
      ->setFunctionField(ctx, getGeneratorCount)
      ->setFunctionField(ctx, setLocalCoord)
      ->setFunctionField(ctx, isLocalCoord)
      ->setFunctionField(ctx, setColor)
      ->setFunctionField(ctx, getColor)
      ->setFunctionField(ctx, setLifetime)
      ->setFunctionField(ctx, getLifetime)
      ->setFunctionField(ctx, setShader)
      ->setFunctionField(ctx, getShader)
      ->setFunctionField(ctx, generateParticle);
}
core::AutoPtr<Value> Trait_ParticleGenerator::create(
    core::AutoPtr<Script> ctx,
    const core::AutoPtr<document::ParticleGenerator> &generator) {
  auto global = ctx->getNativeGlobal();
  auto ParticleGenerator = global->getField(ctx, "ParticleGenerator");
  auto instance = Trait_Node::create(ctx, generator);
  for (auto &field : ParticleGenerator->getKeys(ctx)) {
    instance->setField(ctx, field, ParticleGenerator->getField(ctx, field));
  }
  return instance;
}