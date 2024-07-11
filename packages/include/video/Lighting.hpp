#pragma once
#include "core/Object.hpp"
#include <glm/glm.hpp>
namespace firefly::video {
struct Ambient {
  glm::vec3 color;
  float strength;
};
class Lighting : public core::Object {
private:
  Ambient _ambient;

public:
  Lighting();
  Ambient &getAmbient();
  const Ambient &getAmbient() const;
};
} // namespace firefly::video