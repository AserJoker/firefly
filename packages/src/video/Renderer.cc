#include "video/Renderer.hpp"
#include "core/AutoPtr.hpp"
#include <glad/glad.h>
using namespace firefly;
using namespace firefly::video;
void Renderer::useShader(const core::AutoPtr<Shader> &shader) {
  _shader = shader;
  if (_shader != nullptr) {
    _shader->use();
  }
}
const core::AutoPtr<Shader> &Renderer::getShader() { return _shader; }
void Renderer::clear(const glm::vec4 &color) {
  glClearColor(color.r, color.g, color.b, color.a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
void Renderer::setTextureUnit(const std::string &name, const int32_t id,
                              const core::AutoPtr<Texture> &texture) {
  glActiveTexture(GL_TEXTURE0 + id);
  _shader->setValue(name, id);
  texture->bind();
}
void Renderer::enableDepthTest() { glEnable(GL_DEPTH_TEST); }
void Renderer::disableDepthTest() { glDisable(GL_DEPTH_TEST); }
void Renderer::draw(const core::AutoPtr<Mesh> &mesh) { mesh->draw(_shader); }