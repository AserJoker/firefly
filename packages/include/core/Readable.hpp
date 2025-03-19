#pragma once
#include "Buffer.hpp"
#include "core/AutoPtr.hpp"
#include <cstddef>
namespace firefly::core {
class Readable {
public:
  virtual const core::AutoPtr<Buffer> read(size_t offset = 0,
                                           size_t size = 0) const = 0;
  virtual ~Readable() = default;
  virtual bool isReadable() const = 0;
};
} // namespace firefly::core