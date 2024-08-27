#include "video/Image.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "runtime/Resource.hpp"
#include <SDL_image.h>
#include <SDL_pixels.h>
#include <SDL_rwops.h>
#include <SDL_surface.h>
using namespace firefly;
using namespace firefly::video;
Image::Image(SDL_Surface *surface) { _surface = surface; }
Image::Image(const uint32_t &width, const uint32_t &height,
             const uint32_t &depth, const SDL_PixelFormatEnum &format) {
  _surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, depth, format);
};
Image::Image(core::AutoPtr<runtime::Resource> &reader) {
  auto buffer = reader->read();
  _surface =
      IMG_Load_RW(SDL_RWFromConstMem(buffer->getData(), buffer->getSize()), 1);
}
Image::~Image() {
  if (_surface) {
    SDL_FreeSurface(_surface);
    _surface = nullptr;
  }
}
const std::pair<uint32_t, uint32_t> Image::getSize() const {
  return {_surface->w, _surface->h};
}
const SDL_PixelFormatEnum Image::getFormat() const {
  return (SDL_PixelFormatEnum)_surface->format->format;
}
core::AutoPtr<core::Buffer> Image::getPixels() const {
  return new core::Buffer(_surface->h * _surface->pitch, _surface->pixels);
}
void Image::lock(void **data) { *data = _surface->pixels; }
void Image::unlock() { setVersion(getVersion() + 1); }
core::AutoPtr<Image> Image::convert(const SDL_PixelFormatEnum &format) const {
  return new Image(SDL_ConvertSurfaceFormat(_surface, format, 0));
}