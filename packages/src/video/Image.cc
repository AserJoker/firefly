#include "video/Image.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/Singleton.hpp"
#include "runtime/Media.hpp"
#include "video/ImageFormat.hpp"
#include <SDL_image.h>
#include <SDL_pixels.h>
#include <SDL_rwops.h>
#include <SDL_surface.h>
using namespace firefly;
using namespace firefly::video;

Image::Image(const std::string &path, const IMAGE_FORMAT &format)
    : _format(format) {
  auto media = core::Singleton<runtime::Media>::instance();
  SDL_PixelFormatEnum fmt;
  switch (_format) {
  case IMAGE_FORMAT::RGB:
    _depth = 3;
    fmt = SDL_PIXELFORMAT_BGR24;
    break;
  case IMAGE_FORMAT::RGBA:
    _depth = 4;
    fmt = SDL_PIXELFORMAT_RGBA32;
    break;
  case IMAGE_FORMAT::GRAY:
    fmt = SDL_PIXELFORMAT_INDEX8;
    _depth = 1;
    break;
  }
  auto buf = media->load(path)->read();
  auto src = IMG_Load_RW(SDL_RWFromConstMem(buf->getData(), buf->getSize()), 0);
  if (src->format->format != fmt) {
    auto dst = SDL_ConvertSurfaceFormat(src, fmt, 0);
    SDL_FreeSurface(src);
    src = dst;
  }
  _width = src->w;
  _height = src->h;
  _data = new core::Buffer(_width * _height * _depth, src->pixels);
  SDL_FreeSurface(src);
  setVersion(getVersion() + 1);
  core::AutoPtr updateRangeList = new ImageUpdateList();
  updateRangeList->getData().push_back({0, 0, _width, _height});
  setMetadata("video::update_range_list", updateRangeList);
}
Image::Image(const uint32_t &width, const uint32_t &height,
             const IMAGE_FORMAT &format, const void *data)
    : _width(width), _height(height), _format(format) {
  switch (_format) {
  case IMAGE_FORMAT::RGB:
    _depth = 3;
    break;
  case IMAGE_FORMAT::RGBA:
    _depth = 4;
    break;
  case IMAGE_FORMAT::GRAY:
    _depth = 1;
    break;
  }
  _data = new core::Buffer(_width * _height * _depth, data);
  setVersion(getVersion() + 1);
  core::AutoPtr updateRangeList = new ImageUpdateList();
  updateRangeList->getData().push_back({0, 0, _width, _height});
  setMetadata("video::update_range_list", updateRangeList);
}
const core::AutoPtr<core::Buffer> &Image::getData() const { return _data; }
const uint32_t &Image::getWidth() const { return _width; }
const uint32_t &Image::getHeight() const { return _height; }
const uint32_t &Image::getDepth() const { return _depth; }
const IMAGE_FORMAT Image::getFormat() const { return _format; }
void Image::write(const uint32_t &x, const uint32_t &y, const uint32_t &width,
                  const uint32_t &height, const void *data) {
  if (x >= _width || y >= _height) {
    return;
  }
  auto w = width;
  if (w + x > _width) {
    w = _width - x;
  }
  auto h = height;
  if (h + y > _height) {
    h = _height - y;
  }
  auto pitch = w * _depth;
  for (auto j = 0; j < h; j++) {
    auto dx = x * _depth;
    auto dy = ((j + y));
    auto offset = dy * _width * _depth + dx;
    _data->setData(offset, w * _depth, (const char *)data + j * pitch);
  }
  auto updateRangeList =
      getMetadata("video::update_range_list").cast<ImageUpdateList>();
  updateRangeList->getData().push_back({x, y, w, h});
  setVersion(getVersion() + 1);
}
const core::AutoPtr<core::Buffer> Image::read(const uint32_t &x,
                                              const uint32_t &y,
                                              const uint32_t &width,
                                              const uint32_t &height) const {
  if (x >= _width || y >= _height) {
    return new core::Buffer(0);
  }
  auto raw = (const char *)_data->getData();
  core::AutoPtr<core::Buffer> buf = new core::Buffer(width * height * _depth);
  auto w = width;
  if (w + x > _width) {
    w = _width - x;
  }
  auto h = height;
  if (h + y > _height) {
    h = _height - y;
  }
  auto pitch = w * _depth;
  for (auto j = 0; j < h; j++) {
    auto dx = x * _depth;
    auto dy = ((j + y));
    auto offset = dy * _width * _depth + dx;
    buf->setData(dy * pitch, w * _depth, raw + (dy * pitch + dx));
  }
  return buf;
}
