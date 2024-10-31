#pragma once
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/Object.hpp"
#include "gl/Buffer.hpp"
namespace firefly::video {
class Attribute : public core::Object {
private:
  core::Unsigned_t _itemSize;
  core::Unsigned_t _itemCount;
  core::Unsigned_t _stride;
  core::Boolean_t _normalize;
  core::Boolean_t _dynamic;
  core::AutoPtr<gl::Buffer> _vbo;

public:
  Attribute(const core::AutoPtr<core::Buffer> &buffer,
            core::Unsigned_t itemSize, const core::Boolean_t &normalize = false,
            core::Boolean_t dynamic = false);
  Attribute(core::Unsigned_t size, const void *buffer,
            core::Unsigned_t itemSize, const core::Boolean_t &normalize = false,
            core::Boolean_t dynamic = false);
  template <class T, core::Unsigned_t N>
  Attribute(T (&buffer)[N], core::Unsigned_t itemSize,
            const core::Boolean_t &normalize = false,
            core::Boolean_t dynamic = false)
      : Attribute(sizeof(buffer), buffer, itemSize, normalize, dynamic) {}
  const core::Unsigned_t &getItemSize() const;
  const core::Boolean_t &isNormalized() const;
  const core::Boolean_t &isDynamic() const;
  const core::Unsigned_t &getStride() const;
  const core::Unsigned_t &getItemCount() const;
  const core::AutoPtr<gl::Buffer> &getVertexBufferObject() const;
  void write(core::Unsigned_t offset, core::Unsigned_t size, const void *data);
  template <class T> void write(core::Unsigned_t offset, const T &value) {
    write(offset, sizeof(T), &value);
  }
  void write(core::Unsigned_t size, const void *data);
};
} // namespace firefly::video