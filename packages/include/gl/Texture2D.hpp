#pragma once
#include "TextureFilter.hpp"
#include "TextureWrapMode.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/Cache.hpp"
#include "core/Object.hpp"
#include "core/Size.hpp"
#include "gl/DataType.hpp"
#include "gl/PixelFormat.hpp"

namespace firefly::gl {
class Texture2D : public core::Object, public core::Cache<Texture2D> {
private:
  uint32_t _handle;
  core::Size<> _size;

public:
  Texture2D(uint32_t handle = 0);
  Texture2D(const std::string &name,
            TEXTURE_WRAP_MODE swrap = TEXTURE_WRAP_MODE::REPEAT,
            TEXTURE_WRAP_MODE twrap = TEXTURE_WRAP_MODE::REPEAT);

  Texture2D(uint32_t width, uint32_t height, PIXEL_FORMAT format,
            void *data = 0);

  ~Texture2D() override;

  const uint32_t &getHandle() const;

  void setImage(uint32_t level, PIXEL_FORMAT internalFormat, uint32_t width,
                uint32_t height, PIXEL_FORMAT format, DATA_TYPE type,
                const void *data);

  core::AutoPtr<core::Buffer> getImage(uint32_t level);

  void setSubImage(uint32_t level, uint32_t x, uint32_t y, uint32_t width,
                   uint32_t height, PIXEL_FORMAT format, DATA_TYPE type,
                   const void *data);

  void generateMipmap();

  void setMinifyingFilter(TEXTURE_FILTER filter);

  void setMagnificationFilter(TEXTURE_FILTER filter);

  void setSWrap(TEXTURE_WRAP_MODE mode);

  void setTWrap(TEXTURE_WRAP_MODE mode);

  void setRWrap(TEXTURE_WRAP_MODE mode);

  const core::Size<> &getSize() const;

public:
  static void bind(const core::AutoPtr<Texture2D> &tex, uint32_t index = 0);
};
} // namespace firefly::gl