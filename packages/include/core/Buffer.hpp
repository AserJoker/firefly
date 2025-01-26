#pragma once
#include <cstddef>
namespace firefly::core {
class Buffer {
public:
  virtual const void *getRaw() const = 0;
  virtual size_t getSize() const = 0;
  virtual ~Buffer() = default;
};
} // namespace firefly::core