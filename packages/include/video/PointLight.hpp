#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "gl/Constant.hpp"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
namespace firefly::video {
class PointLight : public core::Object {
private:
  glm::vec3 _position;
  glm::vec3 _color;

public:
  PointLight(const glm::vec3 &position, const glm::vec3 &color);
  void setPosition(const glm::vec3 &position);
  void setColor(const glm::vec3 &color);
  const glm::vec3 &getColor() const;
  const glm::vec3 &getPosition() const;
  void active(core::AutoPtr<gl::Constant> &constants);
};
}; // namespace firefly::video