#include "video/AttributeIndex.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "gl/Buffer.hpp"
#include "gl/BufferUsage.hpp"
using namespace firefly;
using namespace firefly::video;
AttributeIndex::AttributeIndex(const core::AutoPtr<core::Buffer> &buffer)
    : AttributeIndex(buffer->getSize() / sizeof(core::Unsigned_t),
                     (const core::Unsigned_t *)buffer->getData()) {}

AttributeIndex::AttributeIndex(const core::Unsigned_t count,
                               const core::Unsigned_t *buffer) {
  _ebo = new gl::Buffer(gl::BUFFER_USAGE::STATIC_DRAW);
  _size = count;
  _ebo->setData(count * sizeof(core::Unsigned_t), buffer);
}
const core::Unsigned_t AttributeIndex::getIndicesCount() const { return _size; }
void AttributeIndex::write(core::Unsigned_t offset, core::Unsigned_t size,
                           const core::Unsigned_t *data) {
  _ebo->write(offset * sizeof(core::Unsigned_t),
              size * sizeof(core::Unsigned_t), data);
}
const core::AutoPtr<gl::Buffer> &
AttributeIndex::getElementBufferObject() const {
  return _ebo;
}