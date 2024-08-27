#include "gl/Texture2D.hpp"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <glad/glad.h>
using namespace firefly;
using namespace firefly::gl;
Texture2D::Texture2D(const core::AutoPtr<core::Buffer> &image,
                     const uint32_t &handle)
    : Texture(handle) {
  auto img =
      IMG_Load_RW(SDL_RWFromConstMem(image->getData(), image->getSize()), 0);
  auto formattedImg =
      SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_RGBA8888, 0);
  SDL_FreeSurface(img);
  glBindTexture(GL_TEXTURE_2D, getHandle());
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, formattedImg->w, formattedImg->h, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, formattedImg->pixels);
  SDL_FreeSurface(formattedImg);
}
