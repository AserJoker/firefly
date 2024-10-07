#pragma once
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/Object.hpp"
#include "gl/Buffer.hpp"
namespace firefly::video {
class Attribute : public core::Object {
private:
  uint32_t _itemSize;
  uint32_t _itemCount;
  uint32_t _stride;
  bool _normalize;
  bool _dynamic;
  core::AutoPtr<gl::Buffer> _vbo;

public:
  Attribute(const core::AutoPtr<core::Buffer> &buffer, uint32_t itemSize,
            const bool &normalize = false, bool dynamic = false);
  Attribute(uint32_t size, const void *buffer, uint32_t itemSize,
            const bool &normalize = false, bool dynamic = false);
  template <class T, uint32_t N>
  Attribute(T (&buffer)[N], uint32_t itemSize, const bool &normalize = false,
            bool dynamic = false)
      : Attribute(sizeof(buffer), buffer, itemSize, normalize, dynamic) {}
  const uint32_t &getItemSize() const;
  const bool &isNormalized() const;
  const bool &isDynamic() const;
  const uint32_t &getStride() const;
  const uint32_t &getItemCount() const;
  const core::AutoPtr<gl::Buffer> &getVertexBufferObject() const;
  void write(uint32_t offset, uint32_t size, const void *data);
  template <class T> void write(uint32_t offset, const T &value) {
    write(offset, sizeof(T), &value);
  }
};
} // namespace firefly::video