#include "gl/ConstantBlock.hpp"
#include "core/AutoPtr.hpp"
#include "gl/Buffer.hpp"
#include "gl/BufferUsage.hpp"
using namespace firefly;
using namespace firefly::gl;
ConstantBlock::ConstantBlock(const uint32_t &size, const uint32_t &index)
    : _index(index) {
  _ubo = new gl::Buffer(gl::BUFFER_USAGE::STATIC_DRAW);
  _ubo->setData(size, 0);
  gl::Buffer::bindBase(gl::BUFFER_TARGET::UNIFORM, _index, _ubo);
}
const core::AutoPtr<gl::Buffer> &ConstantBlock::getUniformBlockObject() const {
  return _ubo;
}
void ConstantBlock::write(const uint32_t &offset, const uint32_t &size,
                          const void *data) {
  _ubo->write(offset, size, data);
}
const uint32_t &ConstantBlock::getIndex() const { return _index; }