#pragma once
#include "video/VBuffer.hpp"
#include <vector>
namespace firefly::video {
struct Face {
  int indices[3];
};
class ElementBuffer : public VBuffer {
public:
  ElementBuffer(const std::vector<Face> &data,
                const VBufferUsage &usage = VBufferUsage::STATIC_DRAW);
};
}; // namespace firefly::video