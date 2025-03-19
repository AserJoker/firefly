#pragma once
#include "Buffer.hpp"
#include "core/AutoPtr.hpp"
#include <cstddef>
namespace firefly::core {
class Writable {
public:
  virtual void write(const core::AutoConstPtr<Buffer> &buffer,
                     size_t offset = 0) = 0;
  virtual ~Writable() = default;
  virtual bool isWritable() const = 0;
};
} // namespace firefly::core