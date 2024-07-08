#pragma once
#include "core/Object.hpp"
#include <glm/glm.hpp>
namespace firefly::video {
class Renderer : public core::Object {
public:
  void clear(const glm::vec4 &color);
};
} // namespace firefly::video