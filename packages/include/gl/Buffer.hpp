#pragma once
#include "MapAccess.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "gl/BufferTarget.hpp"
#include "gl/BufferUsage.hpp"
namespace firefly::gl {
class Buffer : public core::Object {
private:
  uint32_t _handle;
  BUFFER_USAGE _usage;

public:
  Buffer(BUFFER_USAGE usage, uint32_t handle = 0);
  ~Buffer() override;
  void setData(uint32_t size, const void *data);
  void write(uint32_t offset, uint32_t size, const void *data);
  const void *map(MAP_ACCESS access) const;
  void *map(MAP_ACCESS access);
  void unmap() const;
  const uint32_t getBufferSize() const;

public:
  static void bind(BUFFER_TARGET target, const core::AutoPtr<Buffer> &buffer);
  static void bindBase(BUFFER_TARGET target, uint32_t index,
                       const core::AutoPtr<Buffer> &buffer);
};
} // namespace firefly::gl