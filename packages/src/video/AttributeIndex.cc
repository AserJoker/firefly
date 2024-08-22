#include "video/AttributeIndex.hpp"
#include "core/AutoPtr.hpp"
using namespace firefly;
using namespace firefly::video;

const std::vector<uint32_t> &AttributeIndex::getData() const { return _data; }

std::vector<uint32_t> &AttributeIndex::getData() { return _data; }

void AttributeIndex::write(const uint32_t &offset,
                           const std::vector<uint32_t> &data) {
  if (offset + data.size() > _data.size()) {
    _data.resize(offset + data.size());
  }
  for (auto i = 0; i < data.size(); i++) {
    _data[i + offset] = data[i];
  }
}

const uint32_t &AttributeIndex::read(const uint32_t &offset) {
  return _data[offset];
}
core::AutoPtr<AttributeIndex> AttributeIndex::clone() const {
  core::AutoPtr<AttributeIndex> attri = new AttributeIndex();
  attri->_data = _data;
  return attri;
}