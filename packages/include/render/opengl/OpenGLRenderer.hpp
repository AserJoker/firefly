#pragma once
#include "core/AutoPtr.hpp"
#include "render/Renderer.hpp"
#include "runtime/Window.hpp"

namespace firefly::render {
class OpenGLRenderer : public Renderer {
public:
  OpenGLRenderer();

  ~OpenGLRenderer() override;

  void present() override;

  void initialize(const core::AutoPtr<runtime::Window> &window) override;

  core::AutoPtr<Shader> createShader(const std::string &path) override;

  void activeShader(const core::AutoPtr<Shader> &shader) override;

  void clear(const Color &color) override;
};
} // namespace firefly::render