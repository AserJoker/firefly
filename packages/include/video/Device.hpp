#pragma once
#include "core/Object.hpp"
namespace firefly::video {
class Device : public core::Object {
public:
  void clear();
  void setClearColor(float r, float g, float b, float a);
  void enable(uint32_t state);
  void disable(uint32_t state);
};
}; // namespace firefly::video