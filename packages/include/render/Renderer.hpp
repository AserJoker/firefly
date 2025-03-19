#pragma once
#include "Color.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "render/Shader.hpp"
#include "runtime/Window.hpp"
namespace firefly::render {
class Renderer : public core::Object {
private:
  core::AutoPtr<runtime::Window> _window;

public:
  Renderer();

  ~Renderer() override;

  virtual void initialize(const core::AutoPtr<runtime::Window> &window);

  virtual void present() = 0;

  const core::AutoPtr<runtime::Window> &getWindow() const;

  virtual core::AutoPtr<Shader> createShader(const std::string &path) = 0;

  virtual void activeShader(const core::AutoPtr<Shader> &shader) = 0;

  virtual void clear(const Color &color) = 0;
};
} // namespace firefly::render