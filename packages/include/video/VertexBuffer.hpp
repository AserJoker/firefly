#pragma once
#include "video/VBuffer.hpp"
#include <vector>
namespace firefly::video {
template <class T> class VertexBuffer : public VBuffer {
public:
  VertexBuffer(const std::vector<T> &data,
               const VBufferUsage &usage = VBufferUsage::STATIC_DRAW)
      : VBuffer(data.size() * sizeof(T), (void *)data.data(),
                VBufferType::ARRAY, usage) {}
};
} // namespace firefly::video