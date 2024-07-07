#pragma once
#include "video/IRenderer.hpp"
namespace firefly::video {
class Renderer_OpenGL : public IRenderer {
public:
  void clear(const glm::vec4 &color) override;
};
} // namespace firefly::video