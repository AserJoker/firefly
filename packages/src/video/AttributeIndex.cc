#include "video/AttributeIndex.hpp"
#include "core/AutoPtr.hpp"
using namespace firefly;
using namespace firefly::video;

AttributeIndex::AttributeIndex() {}

const std::vector<uint32_t> &AttributeIndex::getData() const { return _data; }

std::vector<uint32_t> &AttributeIndex::getData() { return _data; }

void AttributeIndex::addUpdateRange(const uint32_t &start,
                                    const uint32_t &count) {
  auto end = start + count;
  for (auto &pair : _updateList) {
    auto &[_start, _count] = pair;
    auto _end = _start + _count;
    if (start >= _start && start <= _end) {
      if (end >= _start && end <= _end) {
        return;
      } else {
        pair.second = end - _start;
        return;
      }
    }
  }
  _updateList.push_back({start, count});
  setVersion(getVersion() + 1);
}

void AttributeIndex::write(const uint32_t &offset,
                           const std::vector<uint32_t> &data) {
  if (offset + data.size() > _data.size()) {
    _data.resize(offset + data.size());
  }
  for (auto i = 0; i < data.size(); i++) {
    _data[i + offset] = data[i];
  }
  addUpdateRange(offset, data.size());
}

const uint32_t &AttributeIndex::read(const uint32_t &offset) {
  return _data[offset];
}

core::AutoPtr<AttributeIndex> AttributeIndex::clone() const {
  core::AutoPtr<AttributeIndex> attri = new AttributeIndex();
  attri->_data = _data;
  return attri;
}

const std::vector<std::pair<uint32_t, uint32_t>> &
AttributeIndex::getUpdateRangeList() const {
  return _updateList;
}
void AttributeIndex::clearUpdateRangeList() {
  _updateList.clear();
}