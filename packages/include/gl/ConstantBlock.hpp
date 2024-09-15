#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "gl/Buffer.hpp"
namespace firefly::gl {
class ConstantBlock : public core::Object {
private:
  uint32_t _index;
  core::AutoPtr<gl::Buffer> _ubo;

public:
  ConstantBlock(uint32_t size, uint32_t index = 0);
  const core::AutoPtr<gl::Buffer> &getUniformBlockObject() const;
  void write(uint32_t offset, uint32_t size, const void *data);
  template <class T> void write(uint32_t offset, const T &value) {
    write(offset, sizeof(T), (void *)&value);
  }
  const uint32_t &getIndex() const;
};
} // namespace firefly::gl