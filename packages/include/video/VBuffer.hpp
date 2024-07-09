#pragma once
#include "core/Object.hpp"
#include <glad/glad.h>
namespace firefly::video {
enum class VBufferType {
  ARRAY = GL_ARRAY_BUFFER,
  ATOMIC_COUNTER = GL_ATOMIC_COUNTER_BUFFER,
  COPY_READ = GL_COPY_READ_BUFFER,
  COPY_WRITE = GL_COPY_WRITE_BUFFER,
  DISPATCH_INDIRECT = GL_DISPATCH_INDIRECT_BUFFER,
  DRAW_INDIRECT = GL_DRAW_INDIRECT_BUFFER,
  ELEMENT_ARRAY = GL_ELEMENT_ARRAY_BUFFER,
  PIXEL_PACK = GL_PIXEL_PACK_BUFFER,
  PIXEL_UNPACK = GL_PIXEL_UNPACK_BUFFER,
  QUERY = GL_QUERY_BUFFER,
  SHADER_STORAGE = GL_SHADER_STORAGE_BUFFER,
  TEXTURE = GL_TEXTURE_BUFFER,
  TRANSFORM_FEEDBACK = GL_TRANSFORM_FEEDBACK_BUFFER,
  UNIFORM = GL_UNIFORM_BUFFER
};
enum class VBufferUsage {
  STREAM_DRAW = GL_STREAM_DRAW,
  STREAM_READ = GL_STREAM_READ,
  STREAM_COPY = GL_STREAM_COPY,
  STATIC_DRAW = GL_STATIC_DRAW,
  STATIC_READ = GL_STATIC_READ,
  STATIC_COPY = GL_STATIC_COPY,
  DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
  DYNAMIC_READ = GL_DYNAMIC_READ,
  DYNAMIC_COPY = GL_DYNAMIC_COPY
};
class VBuffer : public core::Object {
private:
  VBufferType _type;
  VBufferUsage _usage;
  uint32_t _size;
  uint32_t _handle;

public:
  VBuffer(const uint32_t &size, void *data = nullptr,
          const VBufferType &type = VBufferType::ARRAY,
          const VBufferUsage &usage = VBufferUsage::STATIC_DRAW);
  ~VBuffer() override;
  void bind() const;
  void unbind() const;
  const VBufferType &getType() const;
  const VBufferUsage &getUsage() const;
  const uint32_t &getSize() const;
  void lock(size_t *size, void **ppBuffer);
  void unlock();
};
} // namespace firefly::video