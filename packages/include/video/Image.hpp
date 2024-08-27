#pragma once
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/Object.hpp"
#include "runtime/Resource.hpp"
#include <SDL_pixels.h>
#include <SDL_surface.h>
#include <glm/glm.hpp>
namespace firefly::video {
class Image : public core::Object {
private:
  SDL_Surface *_surface;

public:
  Image(SDL_Surface *surface);
  Image(const uint32_t &width, const uint32_t &height, const uint32_t &depth,
        const SDL_PixelFormatEnum &format);
  Image(core::AutoPtr<runtime::Resource> &reader);
  ~Image() override;
  const std::pair<uint32_t, uint32_t> getSize() const;
  const SDL_PixelFormatEnum getFormat() const;
  core::AutoPtr<core::Buffer> getPixels() const;
  void lock(void **data);
  void unlock();
  void setPixel(const uint32_t &x, const uint32_t &y, const uint32_t &color);
  core::AutoPtr<Image> convert(const SDL_PixelFormatEnum &format) const;
};
}; // namespace firefly::video