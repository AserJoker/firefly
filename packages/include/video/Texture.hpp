#pragma once
#include "core/Object.hpp"
namespace firefly::video {
class Texture : public core::Object {
public:
  virtual void bind(const uint32_t &index) = 0;
};
}; // namespace firefly::video