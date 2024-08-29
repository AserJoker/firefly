#pragma once
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/Object.hpp"
namespace firefly::video {
class ConstantBlock : public core::Object {
private:
  core::AutoPtr<core::Buffer> _buffer;
  uint32_t _index;

public:
  ConstantBlock(const uint32_t &index = 0);
  const core::AutoPtr<core::Buffer> &getBuffer() const;
  void write(const uint32_t &offset, const uint32_t &size, const void *data);
  template <class T> void write(const uint32_t &offset, const T &value) {
    write(offset, sizeof(T), (void *)&value);
  }
  const uint32_t &getIndex() const;
};
} // namespace firefly::video