#pragma once
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/Object.hpp"
#include <typeinfo>
namespace firefly::video {
class Attribute : public core::Object {
private:
  core::AutoPtr<core::Buffer> _buffer;
  std::string _itemType;
  uint32_t _itemSize;
  uint32_t _stride;
  bool _normalize;
  bool _dynamic;

public:
  Attribute(const core::AutoPtr<core::Buffer> &buffer,
            const std::type_info &type, const uint32_t &itemSize,
            const bool &normalize = false, const bool &dynamic = false);
  const std::string &getItemType() const;
  const uint32_t &getItemSize() const;
  const bool &isNormalized() const;
  const bool &isDynamic() const;
  const uint32_t &getStride() const;
  const core::AutoPtr<core::Buffer> &getBuffer() const;
  void write(const uint32_t &offset, const uint32_t &size, const void *data);
  template <class T> void write(const uint32_t &offset, const T &value) {
    write(offset, sizeof(T), &value);
  }
};
} // namespace firefly::video