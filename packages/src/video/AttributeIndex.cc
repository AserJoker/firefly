#include "video/AttributeIndex.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "gl/Buffer.hpp"
#include "gl/BufferUsage.hpp"
using namespace firefly;
using namespace firefly::video;
AttributeIndex::AttributeIndex(const core::AutoPtr<core::Buffer> &buffer)
    : _size(0) {
  _ebo = new gl::Buffer(gl::BUFFER_USAGE::STATIC_DRAW);
  _size = buffer->getSize() / sizeof(uint32_t);
  _ebo->setData(buffer->getSize(), buffer->getData());
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