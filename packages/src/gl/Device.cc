#include "gl/Device.hpp"
#include <glad/glad.h>
using namespace firefly;
using namespace firefly::gl;
void Device::setClearColor(const glm::vec4 &color) {
  glClearColor(color.r, color.g, color.b, color.a);
}
void Device::clear(const uint32_t &mode) { glClear(mode); }
void Device::drawElements(const DRAW_MODE &mode, const uint32_t &offset,
                          const uint32_t &count) {
  glDrawElements((GLenum)mode, count, GL_UNSIGNED_INT,
                 (void *)(ptrdiff_t)offset);
}
void Device::activeTextureUnit(const uint32_t &unitId) {
  glActiveTexture(GL_TEXTURE0 + unitId);
}