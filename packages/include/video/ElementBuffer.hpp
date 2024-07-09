#pragma once
#include "video/Buffer.hpp"
#include <vector>
namespace firefly::video {
struct Face {
  int indices[3];
};
class ElementBuffer : public Buffer {
public:
  ElementBuffer(const std::vector<Face> &data,
                const BufferUsage &usage = BufferUsage::STATIC_DRAW);
};
}; // namespace firefly::video