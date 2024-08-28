#include "gl/Texture2D.hpp"
#include <glad/glad.h>
using namespace firefly;
using namespace firefly::gl;
Texture2D::Texture2D(const uint32_t &handle) : _handle(handle) {
  if (!_handle) {
    glGenTextures(1, &_handle);
  }
}
Texture2D::~Texture2D() { glDeleteTextures(1, &_handle); }
void Texture2D::setImage(const uint32_t &level, PIXEL_FORMAT internalFormat,
                         const uint32_t &width, const uint32_t &height,
                         PIXEL_FORMAT format, DATA_TYPE type,
                         const void *data) {
  glBindTexture(GL_TEXTURE_2D, _handle);
  glTexImage2D(GL_TEXTURE_2D, level, (GLint)internalFormat, width, height, 0,
               (GLint)format, (GLenum)type, data);
}
void Texture2D::generateMipmap() {
  glBindTexture(GL_TEXTURE_2D, _handle);
  glGenerateMipmap(GL_TEXTURE_2D);
}
void Texture2D::setMinifyingFilter(TEXTURE_FILTER filter) {
  glBindTexture(GL_TEXTURE_2D, _handle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)filter);
}
void Texture2D::setMagnificationFilter(TEXTURE_FILTER filter) {
  glBindTexture(GL_TEXTURE_2D, _handle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)filter);
}
void Texture2D::setRWrap(TEXTURE_WRAP_MODE mode) {
  glBindTexture(GL_TEXTURE_2D, _handle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, (GLint)mode);
}
void Texture2D::setSWrap(TEXTURE_WRAP_MODE mode) {
  glBindTexture(GL_TEXTURE_2D, _handle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)mode);
}
void Texture2D::setTWrap(TEXTURE_WRAP_MODE mode) {
  glBindTexture(GL_TEXTURE_2D, _handle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)mode);
}
const uint32_t &Texture2D::getHandle() const { return _handle; }
void Texture2D::bind(const core::AutoPtr<Texture2D> &tex) {
  glBindTexture(GL_TEXTURE_2D, tex->_handle);
}