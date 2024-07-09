#include "video/Renderer.hpp"
#include <glad/glad.h>
using namespace firefly;
using namespace firefly::video;
void Renderer::clear(const glm::vec4 &color) {
  glClearColor(color.r, color.g, color.b, color.a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
void Renderer::setTextureUnit(core::AutoPtr<Shader> shader,
                              const std::string &name, const int32_t id,
                              core::AutoPtr<Texture> texture) {
  glActiveTexture(GL_TEXTURE0 + id);
  shader->setValue(name, id);
  texture->bind();
}