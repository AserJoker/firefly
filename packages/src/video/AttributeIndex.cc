#include "video/AttributeIndex.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "gl/Buffer.hpp"
#include "gl/BufferUsage.hpp"
using namespace firefly;
using namespace firefly::video;
AttributeIndex::AttributeIndex(const core::AutoPtr<core::Buffer> &buffer)
    : AttributeIndex(buffer->getSize() / sizeof(uint32_t),
                     (const uint32_t *)buffer->getData()) {}

AttributeIndex::AttributeIndex(const uint32_t count, const uint32_t *buffer) {
  _ebo = new gl::Buffer(gl::BUFFER_USAGE::STATIC_DRAW);
  _size = count;
  _ebo->setData(count * sizeof(uint32_t), buffer);
}
const uint32_t AttributeIndex::getIndicesCount() const { return _size; }
void AttributeIndex::write(uint32_t offset, uint32_t size,
                           const uint32_t *data) {
  _ebo->write(offset * sizeof(uint32_t), size * sizeof(uint32_t), data);
}
const core::AutoPtr<gl::Buffer> &
AttributeIndex::getElementBufferObject() const {
  return _ebo;
}