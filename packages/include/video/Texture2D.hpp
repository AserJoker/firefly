#pragma once
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "video/Texture.hpp"
namespace firefly::video {
class Texture2D : public Texture {
public:
  Texture2D(const core::AutoPtr<core::Buffer> &image,
            const uint32_t &handle = 0);
};
} // namespace firefly::video