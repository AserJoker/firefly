#pragma once
#include "Renderer.hpp"
#include "core/AutoPtr.hpp"
#include "runtime/Window.hpp"
namespace firefly::render {
class OpenGLRenderer : public Renderer {
public:
  OpenGLRenderer();
  
  ~OpenGLRenderer() override;

  void present() override;

  void initialize(const core::AutoPtr<runtime::Window> &window) override;
};
} // namespace firefly::render