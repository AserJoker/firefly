#include "video/PointLight.hpp"
using namespace firefly;
using namespace firefly::video;
PointLight::PointLight(const glm::vec3 &position, const glm::vec3 &color)
    : _position(position), _color(color) {}
void PointLight::setPosition(const glm::vec3 &position) {
  _position = position;
}
void PointLight::setColor(const glm::vec3 &color) { _color = color; }
const glm::vec3 &PointLight::getColor() const { return _color; }
const glm::vec3 &PointLight::getPosition() const { return _position; }
void PointLight::active(core::AutoPtr<gl::Constant> &constants,
                        uint32_t index) {
  constants->setField(fmt::format("pointLights[{}].position", index),
                      _position);
  constants->setField(fmt::format("pointLights[{}].color", index), _color);
}