#pragma once
#include "core/Object.hpp"
namespace firefly::video {
class Texture : public core::Object {
private:
  uint32_t _handle;

public:
  const uint32_t &getHandle() const;
  Texture(const uint32_t &handle = 0);
  ~Texture() override;
};
} // namespace firefly::video