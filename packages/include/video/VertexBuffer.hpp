#pragma once
#include "video/Buffer.hpp"
#include <vector>
namespace firefly::video {
template <class T> class VertexBuffer : public Buffer {
public:
  VertexBuffer(const std::vector<T> &data,
               const BufferUsage &usage = BufferUsage::STATIC_DRAW)
      : Buffer(data.size() * sizeof(T), (void *)data.data(), BufferType::ARRAY,
               usage) {}
};
} // namespace firefly::video