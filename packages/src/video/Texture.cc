#include "video/Texture.hpp"
#include "core/AutoPtr.hpp"
#include "runtime/Resource.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL_pixels.h>
#include <SDL_surface.h>
#include <glad/glad.h>
using namespace firefly::video;
Texture::Texture(core::AutoPtr<runtime::Resource> resource) : _handle(0) {
  auto data = resource->read();
  SDL_Surface *surface =
      IMG_Load_RW(SDL_RWFromConstMem(data->getData(), data->getSize()), 0);
  if (surface->format->format != SDL_PIXELFORMAT_ABGR8888) {
    SDL_Surface *image =
        SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(surface);
    surface = image;
  }
  glGenTextures(1, &_handle);
  bind();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, surface->pixels);
  glGenerateMipmap(GL_TEXTURE_2D);
  unbind();
  SDL_FreeSurface(surface);
}
Texture::~Texture() { glDeleteTextures(1, &_handle); }
void Texture::bind() const { glBindTexture(GL_TEXTURE_2D, _handle); }
void Texture::unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }