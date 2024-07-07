#include "video/Renderer_OpenGL.hpp"
#include <glad/glad.h>
using namespace firefly;
using namespace firefly::video;
void Renderer_OpenGL::clear(const glm::vec4 &color) {
  glClearColor(color.r, color.g, color.b, color.a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}