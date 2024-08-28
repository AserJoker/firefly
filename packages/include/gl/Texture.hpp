#pragma once
#include "TextureFilter.hpp"
#include "TextureWrapMode.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "gl/DataType.hpp"
#include "gl/PixelFormat.hpp"
#include "gl/TextureTarget.hpp"
namespace firefly::gl {
class Texture : public core::Object {
private:
  uint32_t _handle;

public:
  const uint32_t &getHandle() const;
  Texture(const uint32_t &handle = 0);
  ~Texture() override;
  void set2DTextureImage(const uint32_t &level, PIXEL_FORMAT internalFormat,
                         const uint32_t &width, const uint32_t &height,
                         PIXEL_FORMAT format, DATA_TYPE type, const void *data);
  void generateMipmap();

  void setMinifyingFilter(TEXTURE_FILTER filter);
  void setMagnificationFilter(TEXTURE_FILTER filter);
  void setSWrap(TEXTURE_WRAP_MODE mode);
  void setRWrap(TEXTURE_WRAP_MODE mode);
  void setTWrap(TEXTURE_WRAP_MODE mode);

public:
  static void bind(TEXTURE_TARGET target, const core::AutoPtr<Texture> &tex);
};
} // namespace firefly::gl