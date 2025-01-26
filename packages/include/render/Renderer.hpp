#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
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
};
} // namespace firefly::render