#include "video/AmbientLight.hpp"
using namespace firefly;
using namespace firefly::video;
AmbientLight::AmbientLight(float strength, const glm::vec3 &color)
    : _strength(strength), _color(color) {}
float AmbientLight::getStrength() const { return _strength; }
const glm::vec3 &AmbientLight::getColor() const { return _color; }
void AmbientLight::setStrength(float strength) { _strength = strength; }
void AmbientLight::setColor(const glm::vec3 &color) { _color = color; }
void AmbientLight::active(core::AutoPtr<gl::Constant> &constants) const {
  constants->setField("ambientLight.strength", _strength);
  constants->setField("ambientLight.color", _color);
}