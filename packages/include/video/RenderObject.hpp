#pragma once
#include "core/Object.hpp"
namespace firefly::video {
class RenderObject : public core::Object {
public:
  virtual void draw() = 0;
  void enable();
  void disable();
};
} // namespace firefly::video