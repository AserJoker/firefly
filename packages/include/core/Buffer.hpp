#pragma once
#include "core/Object.hpp"
namespace firefly::core {
class Buffer : public Object {
private:
  void *_data;
  uint32_t _size;

public:
  Buffer(const uint32_t &size, void *data = 0);
  ~Buffer() override;
  const void *getData() const;
  const uint32_t &getSize() const;
  void setData(const uint32_t &offset, const uint32_t &length, void *data);
};
} // namespace firefly::core