#pragma once
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/Object.hpp"
#include "gl/Buffer.hpp"
namespace firefly::video {
class AttributeIndex : public core::Object {
private:
  core::AutoPtr<gl::Buffer> _ebo;
  core::Unsigned_t _size;

public:
  AttributeIndex(const core::AutoPtr<core::Buffer> &buffer);
  AttributeIndex(const core::Unsigned_t count, const core::Unsigned_t *buffer);
  template <core::Unsigned_t N>
  AttributeIndex(const core::Unsigned_t (&buffer)[N])
      : AttributeIndex(N, buffer) {}
  const core::Unsigned_t getIndicesCount() const;
  void write(core::Unsigned_t offset, core::Unsigned_t size,
             const core::Unsigned_t *data);
  const core::AutoPtr<gl::Buffer> &getElementBufferObject() const;
};
}; // namespace firefly::video