#pragma once
#include "document/ParticleGenerator.hpp"
#include "script/Script.hpp"
namespace firefly::script {
class Trait_ParticleGenerator {
private:
  static FUNC_DEF(setTexture);
  static FUNC_DEF(getTexture);
  static FUNC_DEF(setDirection);
  static FUNC_DEF(getDirection);
  static FUNC_DEF(setSpeed);
  static FUNC_DEF(getSpeed);
  static FUNC_DEF(setLinearAcceleration);
  static FUNC_DEF(getLinearAcceleration);
  static FUNC_DEF(setTangentialAcceleration);
  static FUNC_DEF(getTangentialAcceleration);
  static FUNC_DEF(setRadialAcceleration);
  static FUNC_DEF(getRadialAcceleration);
  static FUNC_DEF(setSize);
  static FUNC_DEF(getSize);
  static FUNC_DEF(setPosition);
  static FUNC_DEF(getPosition);
  static FUNC_DEF(setDelay);
  static FUNC_DEF(getDelay);
  static FUNC_DEF(setRandomAngle);
  static FUNC_DEF(getRandomAngle);
  static FUNC_DEF(setGeneratorCount);
  static FUNC_DEF(getGeneratorCount);
  static FUNC_DEF(setLocalCoord);
  static FUNC_DEF(isLocalCoord);
  static FUNC_DEF(setColor);
  static FUNC_DEF(getColor);
  static FUNC_DEF(setLifetime);
  static FUNC_DEF(getLifetime);
  static FUNC_DEF(setShader);
  static FUNC_DEF(getShader);
  static FUNC_DEF(generateParticle);

public:
  static void initialize(core::AutoPtr<Script> script);
  static core::AutoPtr<Value>
  create(core::AutoPtr<Script> ctx,
         const core::AutoPtr<document::ParticleGenerator> &generator);
};
}; // namespace firefly::script