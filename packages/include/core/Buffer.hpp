#pragma once
#include "core/Object.hpp"
namespace firefly::core {
class Buffer : public Object {
private:
  void *_data;
  uint32_t _size;

public:
  Buffer(uint32_t size = 0, const void *data = 0);
  ~Buffer() override;
  const void *getData() const;
  const uint32_t &getSize() const;
  void setData(uint32_t offset, uint32_t length, const void *data);
};
} // namespace firefly::core