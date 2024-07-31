#include "video/Renderer.hpp"
#include <glad/glad.h>
using namespace firefly;
using namespace firefly::video;
void Renderer::initialize() {}
void Renderer::clear() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
void Renderer::render() {
  clear();
  for (auto &[_, obj] : _robjects) {
    obj->draw();
  }
}
void Renderer::setClearColor(float r, float g, float b, float a) {
  glClearColor(r, g, b, a);
}