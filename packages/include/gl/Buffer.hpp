#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "gl/BufferTarget.hpp"
#include "gl/BufferUsage.hpp"
namespace firefly::gl {
class Buffer : public core::Object {
private:
  uint32_t _handle;
  uint32_t _size;
  BUFFER_USAGE _usage;

public:
  Buffer(BUFFER_USAGE usage, const uint32_t &handle = 0);
  ~Buffer() override;
  void setData(const uint32_t &size, void *data);
  void write(const uint32_t &offset, const uint32_t &size, void *data);

public:
  static void bind(BUFFER_TARGET target, const core::AutoPtr<Buffer> &buffer);
};
} // namespace firefly::gl