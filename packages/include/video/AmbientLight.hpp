#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "gl/Constant.hpp"
#include <glm/glm.hpp>
namespace firefly::video {
class AmbientLight : public core::Object {
private:
  float _strength;
  glm::vec3 _color;

public:
  AmbientLight(float strength = 1.0f,
               const glm::vec3 &color = {1.0f, 1.0f, 1.0f});
  float getStrength() const;
  const glm::vec3 &getColor() const;
  void setStrength(float strength);
  void setColor(const glm::vec3 &color);
  void active(core::AutoPtr<gl::Constant> &constants) const;
};
}; // namespace firefly::video