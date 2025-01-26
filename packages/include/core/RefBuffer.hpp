#pragma once
#include "Buffer.hpp"
#include "core/AutoPtr.hpp"
namespace firefly::core {
class RefBuffer : public Buffer {
private:
  core::AutoConstPtr<Buffer> _source;
  size_t _offset;
  size_t _size;

public:
  RefBuffer(const core::AutoConstPtr<Buffer> &buffer, size_t offset = 0,
            size_t size = 0);
  
  const void *getRaw() const override;

  size_t getSize() const override;
};
}; // namespace firefly::core