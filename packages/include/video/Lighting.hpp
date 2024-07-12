#pragma once
#include "core/Object.hpp"
#include <glm/glm.hpp>
namespace firefly::video {
class Lighting : public core::Object {
private:
  glm::vec3 _position;
  glm::vec3 _ambient;
  glm::vec3 _diffuse;
  glm::vec3 _specular;
  float _shininess;

public:
  Lighting();
  glm::vec3 &getPosition();
  const glm::vec3 &getPositon() const;
  glm::vec3 &getAmbient();
  const glm::vec3 &getAmbient() const;
  glm::vec3 &getDiffuse();
  const glm::vec3 &getDiffuse() const;
  glm::vec3 &getSpecular();
  const glm::vec3 &getSpecular() const;
  float &getShininess();
  const float &getShininess() const;
};
} // namespace firefly::video