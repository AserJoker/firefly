#include "render/Renderer.hpp"
#include "core/AutoPtr.hpp"
#include "runtime/Window.hpp"
using namespace firefly;
using namespace firefly::render;
Renderer::Renderer() {}

void Renderer::initialize(const core::AutoPtr<runtime::Window> &window) {
  _window = window;
}

Renderer::~Renderer() {}

const core::AutoPtr<runtime::Window> &Renderer::getWindow() const {
  return _window;
}