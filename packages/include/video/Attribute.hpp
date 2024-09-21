#pragma once
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/Object.hpp"
#include "gl/Buffer.hpp"
#include <typeinfo>
namespace firefly::video {
class Attribute : public core::Object {
private:
  std::string _itemType;
  uint32_t _itemSize;
  uint32_t _itemCount;
  uint32_t _stride;
  bool _normalize;
  bool _dynamic;
  core::AutoPtr<gl::Buffer> _vbo;

public:
  Attribute(const core::AutoPtr<core::Buffer> &buffer,
            const std::type_info &type, uint32_t itemSize,
            const bool &normalize = false, bool dynamic = false);
  Attribute(uint32_t size, const void *buffer, const std::type_info &type,
            uint32_t itemSize, const bool &normalize = false,
            bool dynamic = false);
  template <class T, uint32_t N>
  Attribute(T (&buffer)[N], uint32_t itemSize, const bool &normalize = false,
            bool dynamic = false)
      : Attribute(N * sizeof(T), buffer, typeid(T), itemSize, normalize,
                  dynamic) {}
  const std::string &getItemType() const;
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