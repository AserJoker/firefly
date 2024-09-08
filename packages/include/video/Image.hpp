#pragma once
#include "ImageFormat.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/Cache.hpp"
#include "core/Object.hpp"
#include "core/Pack.hpp"
#include <glm/glm.hpp>
#include <vector>
namespace firefly::video {
using ImageUpdateList = core::Pack<std::vector<glm::ivec4>>;
class Image : public core::Object, public core::Cache<Image> {
private:
  core::AutoPtr<core::Buffer> _data;
  uint32_t _width;
  uint32_t _height;
  uint32_t _depth;
  IMAGE_FORMAT _format;

public:
  Image(const std::string &path,
        const IMAGE_FORMAT &format = IMAGE_FORMAT::RGBA);
  Image(const uint32_t &width, const uint32_t &height,
        const IMAGE_FORMAT &format, const void *data = 0);
  const core::AutoPtr<core::Buffer> &getData() const;
  const uint32_t &getWidth() const;
  const uint32_t &getHeight() const;
  const uint32_t &getDepth() const;
  const IMAGE_FORMAT getFormat() const;
  void write(const uint32_t &x, const uint32_t &y, const uint32_t &width,
             const uint32_t &height, const void *data);
  const core::AutoPtr<core::Buffer> read(const uint32_t &x, const uint32_t &y,
                                         const uint32_t &width,
                                         const uint32_t &height) const;
};
}; // namespace firefly::video