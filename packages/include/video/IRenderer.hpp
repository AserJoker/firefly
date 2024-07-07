#pragma once
#include "core/Object.hpp"
#include <glm/glm.hpp>
namespace firefly::video {
class IRenderer : public core::Object {
public:
  virtual void clear(const glm::vec4 &color) = 0;
};
} // namespace firefly::video