#pragma once
#include "core/Object.hpp"
#include "gl/DrawMode.hpp"
#include <glm/glm.hpp>
namespace firefly::gl {
class Device : public core::Object {
public:
  void setClearColor(const glm::vec4 &color);
  void clear(const uint32_t &mode);
  void drawElements(const DRAW_MODE &mode, const uint32_t &offset,
                    const uint32_t &count);
  void activeTextureUnit(const uint32_t &unitId);
};
}; // namespace firefly::gl