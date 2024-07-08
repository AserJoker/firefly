#pragma once
#include "core/Object.hpp"
#include <glad/glad.h>
namespace firefly::video {
enum class BufferType {
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
enum class BufferUsage {
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
class Buffer : public core::Object {
private:
  BufferType _type;
  BufferUsage _usage;
  uint32_t _size;
  uint32_t _handle;

public:
  Buffer(const uint32_t &size, void *data = nullptr,
         const BufferType &type = BufferType::ARRAY,
         const BufferUsage &usage = BufferUsage::STATIC_DRAW);
  ~Buffer() override;
  void bind() const;
  void unbind() const;
  const BufferType &getType() const;
  const BufferUsage &getUsage() const;
  const uint32_t &getSize() const;
};
} // namespace firefly::video