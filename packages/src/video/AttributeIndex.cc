#include "video/AttributeIndex.hpp"
#include "core/Buffer.hpp"
#include "video/UpdateRangeList.hpp"
using namespace firefly;
using namespace firefly::video;
AttributeIndex::AttributeIndex() {
  _buffer = new core::Buffer(0);
  core::AutoPtr updateRangeList = new UpdateRangeList();
  setMetadata("video::update_range_list", updateRangeList);
}
const uint32_t *AttributeIndex::getIndices() const {
  return (uint32_t *)_buffer->getData();
}
const uint32_t AttributeIndex::getIndicesCount() const {
  return (uint32_t)(_buffer->getSize() / sizeof(uint32_t));
}
void AttributeIndex::write(const uint32_t &offset, const uint32_t &size,
                           const uint32_t *data) {
  _buffer->setData(offset * sizeof(uint32_t), size * sizeof(uint32_t), data);
  core::AutoPtr updateRangeList = new UpdateRangeList();
  updateRangeList->add(offset * sizeof(uint32_t), size * sizeof(uint32_t));
  setVersion(getVersion() + 1);
}