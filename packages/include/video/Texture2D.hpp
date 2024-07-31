#pragma once
#include "Texture.hpp"
namespace firefly::video {
class Texture2D : public Texture {
public:
  void bind(const uint32_t &idx) override;
};
}; // namespace firefly::video