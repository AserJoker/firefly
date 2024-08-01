#include "video/Texture2D.hpp"
#include "core/Singleton.hpp"
#include "video/Renderer.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL_pixels.h>
#include <SDL_rwops.h>
#include <SDL_surface.h>
#include <glad/glad.h>
using namespace firefly;
using namespace firefly::video;
Texture2D::Texture2D() { glGenTextures(1, &_handle); }
void Texture2D::bind(const uint32_t &idx) {
  auto renderer = core::Singleton<Renderer>::instance();
  auto shader = renderer->getCurrentShader();
  glActiveTexture(GL_TEXTURE0 + idx);
  glBindTexture(GL_TEXTURE_2D, _handle);
  shader->setUniform(fmt::format("texture_{}", idx), (int32_t)_handle);
}
void Texture2D::setImage(const core::AutoPtr<core::Buffer> &buffer) {
  SDL_Surface *surface =
      IMG_Load_RW(SDL_RWFromConstMem(buffer->getData(), buffer->getSize()), 0);
  SDL_Surface *surface_result =
      SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ABGR8888, 0);
  setImage(surface_result->w, surface_result->h, GL_RGBA,
           surface_result->pixels);
  SDL_FreeSurface(surface_result);
  SDL_FreeSurface(surface);
}
void Texture2D::setImage(const uint32_t width, const uint32_t height,
                         uint32_t format, void *buffer) {
  glBindTexture(GL_TEXTURE_2D, _handle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format,
               GL_UNSIGNED_BYTE, buffer);
  glGenerateMipmap(GL_TEXTURE_2D);
}
const Geometry::Size Texture2D::getTextureSize() const {
  int w, h;
  glBindTexture(GL_TEXTURE_2D, _handle);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
  return {(float)w, (float)h};
}