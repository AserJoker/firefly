#pragma once
#include "Geometry.hpp"
#include "Texture.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
namespace firefly::video {
class Texture2D : public Texture {
private:
  uint32_t _handle;

public:
  Texture2D();
  void setImage(const core::AutoPtr<core::Buffer> &buffer);
  void setImage(const uint32_t width, const uint32_t height, uint32_t format,
                void *buffer);
  const Geometry::Size getTextureSize() const;
  void bind(const uint32_t &idx) override;
};
}; // namespace firefly::video