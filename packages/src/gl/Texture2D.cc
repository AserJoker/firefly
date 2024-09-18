#include "gl/Texture2D.hpp"
#include "core/Singleton.hpp"
#include "gl/PixelFormat.hpp"
#include "runtime/Media.hpp"
#include <SDL_image.h>
#include <glad/glad.h>
using namespace firefly;
using namespace firefly::gl;
Texture2D::Texture2D(uint32_t handle) : _handle(handle) {
  if (!_handle) {
    glGenTextures(1, &_handle);
  }
}
Texture2D::Texture2D(const std::string &name, TEXTURE_WRAP_MODE swrap,
                     TEXTURE_WRAP_MODE twrap)
    : Texture2D() {
  auto media = core::Singleton<runtime::Media>::instance();
  auto buf = media->load(name)->read();
  SDL_Surface *img =
      IMG_Load_RW(SDL_RWFromConstMem(buf->getData(), buf->getSize()), 1);
  PIXEL_FORMAT fmt;
  switch ((SDL_PixelFormatEnum)img->format->format) {
  case SDL_PIXELFORMAT_RGBA4444:
    fmt = PIXEL_FORMAT::RGBA16;
    break;
  case SDL_PIXELFORMAT_RGB24:
    fmt = PIXEL_FORMAT::RGB;
    break;
  case SDL_PIXELFORMAT_RGBA8888:
  case SDL_PIXELFORMAT_RGBX8888:
    fmt = PIXEL_FORMAT::RGBA;
    break;
  case SDL_PIXELFORMAT_INDEX8:
    fmt = PIXEL_FORMAT::RED;
    break;
  default: {
    SDL_Surface *sur = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(img);
    img = sur;
    fmt = PIXEL_FORMAT::RGBA;
    break;
  }
  }
  glBindTexture(GL_TEXTURE_2D, _handle);
  glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)fmt, img->w, img->h, 0, (GLenum)fmt,
               GL_UNSIGNED_BYTE, img->pixels);
  glBindTexture(GL_TEXTURE_2D, 0);
  generateMipmap();
  setSWrap(swrap);
  setTWrap(twrap);
  SDL_FreeSurface(img);
}

Texture2D::Texture2D(uint32_t width, uint32_t height, PIXEL_FORMAT format,
                     void *data) {

  glGenTextures(1, &_handle);
  glBindTexture(GL_TEXTURE_2D, _handle);
  glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)format, width, height, 0,
               (GLenum)format, GL_UNSIGNED_BYTE, data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
Texture2D::~Texture2D() { glDeleteTextures(1, &_handle); }
void Texture2D::setImage(uint32_t level, PIXEL_FORMAT internalFormat,
                         uint32_t width, uint32_t height, PIXEL_FORMAT format,
                         DATA_TYPE type, const void *data) {
  glBindTexture(GL_TEXTURE_2D, _handle);
  glTexImage2D(GL_TEXTURE_2D, level, (GLint)internalFormat, width, height, 0,
               (GLint)format, (GLenum)type, data);
}
void Texture2D::setSubImage(uint32_t level, uint32_t x, uint32_t y,
                            uint32_t width, uint32_t height,
                            PIXEL_FORMAT format, DATA_TYPE type,
                            const void *data) {
  glBindTexture(GL_TEXTURE_2D, _handle);
  glTexSubImage2D(GL_TEXTURE_2D, level, x, y, width, height, (GLenum)format,
                  (GLenum)type, data);
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