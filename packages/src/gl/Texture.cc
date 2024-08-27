#include "gl/Texture.hpp"
#include <glad/glad.h>
using namespace firefly;
using namespace firefly::gl;
Texture::Texture(const uint32_t &handle) : _handle(handle) {
  if (!_handle) {
    glGenTextures(1, &_handle);
  }
}
Texture::~Texture() { glDeleteTextures(1, &_handle); }
const uint32_t &Texture::getHandle() const { return _handle; }