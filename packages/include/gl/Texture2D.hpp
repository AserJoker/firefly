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
  core::Unsigned_t _handle;
  core::Size<> _size;

public:
  Texture2D(core::Unsigned_t handle = 0);
  Texture2D(const core::String_t &name,
            TEXTURE_WRAP_MODE swrap = TEXTURE_WRAP_MODE::REPEAT,
            TEXTURE_WRAP_MODE twrap = TEXTURE_WRAP_MODE::REPEAT);

  Texture2D(core::Unsigned_t width, core::Unsigned_t height,
            PIXEL_FORMAT format, void *data = 0);

  ~Texture2D() override;

  const core::Unsigned_t &getHandle() const;

  void setImage(core::Unsigned_t level, PIXEL_FORMAT internalFormat,
                core::Unsigned_t width, core::Unsigned_t height,
                PIXEL_FORMAT format, DATA_TYPE type, const void *data);

  core::AutoPtr<core::Buffer> getImage(core::Unsigned_t level,
                                       PIXEL_FORMAT fmt = PIXEL_FORMAT::RGBA);

  void setSubImage(core::Unsigned_t level, core::Unsigned_t x,
                   core::Unsigned_t y, core::Unsigned_t width,
                   core::Unsigned_t height, PIXEL_FORMAT format, DATA_TYPE type,
                   const void *data);

  void generateMipmap();

  void setMinifyingFilter(TEXTURE_FILTER filter);

  void setMagnificationFilter(TEXTURE_FILTER filter);

  void setSWrap(TEXTURE_WRAP_MODE mode);

  void setTWrap(TEXTURE_WRAP_MODE mode);

  void setRWrap(TEXTURE_WRAP_MODE mode);

  const core::Size<> &getSize() const;

public:
  static void bind(const core::AutoPtr<Texture2D> &tex,
                   core::Unsigned_t index = 0);
};
} // namespace firefly::gl