#pragma once
#include "Resource.hpp"
namespace firefly::runtime {
class Resource_Buffer : public Resource {
private:
  void *_buffer;
  uint32_t _size;
  uint32_t _pos;

public:
  Resource_Buffer(const void *buffer = 0, const uint32_t &size = 0);
  ~Resource_Buffer() override;
  void *read(const size_t &size = 0, size_t *len = 0) override;
  void write(void *buf, const size_t &size) override;
};
} // namespace firefly::runtime