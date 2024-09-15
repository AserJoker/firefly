#pragma once
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/Object.hpp"
#include "gl/Buffer.hpp"
namespace firefly::video {
class AttributeIndex : public core::Object {
private:
  core::AutoPtr<gl::Buffer> _ebo;
  uint32_t _size;

public:
  AttributeIndex(const core::AutoPtr<core::Buffer> &buffer);
  const uint32_t getIndicesCount() const;
  void write(uint32_t offset, uint32_t size, const uint32_t *data);
  const core::AutoPtr<gl::Buffer> &getElementBufferObject() const;
};
}; // namespace firefly::video