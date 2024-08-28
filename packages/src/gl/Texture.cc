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
void Texture::set2DTextureImage(const uint32_t &level,
                                PIXEL_FORMAT internalFormat,
                                const uint32_t &width, const uint32_t &height,
                                PIXEL_FORMAT format, DATA_TYPE type,
                                const void *data) {
  glBindTexture(GL_TEXTURE_2D, _handle);
  glTexImage2D(GL_TEXTURE_2D, level, (GLint)internalFormat, width, height, 0,
               (GLint)format, (GLenum)type, data);
}
void Texture::generateMipmap() {
  glBindTexture(GL_TEXTURE_2D, _handle);
  glGenerateMipmap(GL_TEXTURE_2D);
}
void Texture::setMinifyingFilter(TEXTURE_FILTER filter) {
  glBindTexture(GL_TEXTURE_2D, _handle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)filter);
}
void Texture::setMagnificationFilter(TEXTURE_FILTER filter) {
  glBindTexture(GL_TEXTURE_2D, _handle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)filter);
}
void Texture::setRWrap(TEXTURE_WRAP_MODE mode) {
  glBindTexture(GL_TEXTURE_2D, _handle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, (GLint)mode);
}
void Texture::setSWrap(TEXTURE_WRAP_MODE mode) {
  glBindTexture(GL_TEXTURE_2D, _handle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)mode);
}
void Texture::setTWrap(TEXTURE_WRAP_MODE mode) {
  glBindTexture(GL_TEXTURE_2D, _handle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)mode);
}
const uint32_t &Texture::getHandle() const { return _handle; }
void Texture::bind(TEXTURE_TARGET target, const core::AutoPtr<Texture> &tex) {
  glBindTexture((GLenum)target, tex->_handle);
}