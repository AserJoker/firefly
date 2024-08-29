#include "video/ConstantBlock.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "video/UpdateRangeList.hpp"
using namespace firefly;
using namespace firefly::video;
ConstantBlock::ConstantBlock(const uint32_t &index) : _index(index) {
  _buffer = new core::Buffer(0);
  core::AutoPtr updateRangeList = new UpdateRangeList();
  setMetadata("video::update_range_list", updateRangeList);
}

const core::AutoPtr<core::Buffer> &ConstantBlock::getBuffer() const {
  return _buffer;
}
void ConstantBlock::write(const uint32_t &offset, const uint32_t &size,
                          const void *data) {
  _buffer->setData(offset, size, data);
  auto updateRangeList =
      getMetadata("video::update_range_list").cast<UpdateRangeList>();
  updateRangeList->add(offset, size);
  setVersion(getVersion() + 1);
}
const uint32_t &ConstantBlock::getIndex() const { return _index; }