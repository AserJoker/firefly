#pragma once
#include "Resource.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
namespace firefly::runtime {
class Resource_Buffer : public Resource {
private:
  void *_buffer;
  uint32_t _size;
  uint32_t _pos;

public:
  Resource_Buffer(const void *buffer = 0, uint32_t size = 0);
  ~Resource_Buffer() override;
  core::AutoPtr<core::Buffer> read(uint32_t size) override;
  void write(const core::AutoPtr<core::Buffer> &data) override;
};
} // namespace firefly::runtime