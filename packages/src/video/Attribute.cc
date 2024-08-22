#include "video/Attribute.hpp"
#include "gl/DataType.hpp"
#include <cstdint>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
using namespace firefly;
using namespace firefly::video;
Attribute::Attribute(const std::vector<float> &data, const uint32_t &itemSize,
                     const gl::BUFFER_USAGE &usage,
                     const gl::DATA_TYPE &dataType, const bool &normalized)
    : _data(data), _itemSize(itemSize), _usage(usage), _dataType(dataType),
      _normalized(normalized) {
  _count = _data.size() / _itemSize;
  addUpdateRange(0, _count);
}

Attribute::Attribute(const std::vector<glm::vec2> &data,
                     const gl::BUFFER_USAGE &usage, const bool &normalized)
    : Attribute({}, 2, usage, gl::DATA_TYPE::FLOAT, normalized) {
  for (auto i = 0; i < data.size(); i++) {
    _data.push_back(data[i][0]);
    _data.push_back(data[i][1]);
  }
}

Attribute::Attribute(const std::vector<glm::vec3> &data,
                     const gl::BUFFER_USAGE &usage, const bool &normalized)
    : Attribute({}, 3, usage, gl::DATA_TYPE::FLOAT, normalized) {
  for (auto i = 0; i < data.size(); i++) {
    _data.push_back(data[i][0]);
    _data.push_back(data[i][1]);
    _data.push_back(data[i][2]);
  }
}

Attribute::Attribute(const std::vector<glm::vec4> &data,
                     const gl::BUFFER_USAGE &usage, const bool &normalized)
    : Attribute({}, 4, usage, gl::DATA_TYPE::FLOAT, normalized) {
  for (auto i = 0; i < data.size(); i++) {
    _data.push_back(data[i][0]);
    _data.push_back(data[i][1]);
    _data.push_back(data[i][2]);
    _data.push_back(data[i][3]);
  }
}

void Attribute::addUpdateRange(const uint32_t &start, const uint32_t &count) {
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
}

const uint32_t &Attribute::getItemSize() const { return _itemSize; }
const bool &Attribute::isNormalized() const { return _normalized; }
const gl::BUFFER_USAGE &Attribute::getUsage() const { return _usage; }
const gl::DATA_TYPE &Attribute::getDataType() const { return _dataType; }
const std::vector<float> &Attribute::getData() const { return _data; }
std::vector<float> &Attribute::getData() { return _data; }

void Attribute::write(const uint32_t &offset, const std::vector<float> &data) {
  if (offset + data.size() > _data.size()) {
    _data.resize(offset + data.size());
  }
  for (auto i = 0; i < data.size(); i++) {
    _data[i + offset] = data[i];
  }
  addUpdateRange(offset, data.size() / _itemSize);
}
void *Attribute::read(const uint32_t &index) {
  if (index >= _count) {
    return nullptr;
  }
  return _data.data() + index;
}

const AttributeUpdateRangeList &Attribute::getUpdateRangeList() const {
  return _updateList;
}

void Attribute::clearUpdateRangeList() { _updateList.clear(); }

core::AutoPtr<Attribute> Attribute::clone() const {
  return new Attribute(_data, _itemSize, _usage, _dataType, _normalized);
}