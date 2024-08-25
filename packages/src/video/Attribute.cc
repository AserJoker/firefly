#include "video/Attribute.hpp"
#include "gl/DataType.hpp"
#include <cstdint>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
using namespace firefly;
using namespace firefly::video;
Attribute::Attribute(const std::vector<char> &data, const uint32_t &itemSize,
                     const gl::BUFFER_USAGE &usage,
                     const gl::DATA_TYPE &dataType, const bool &normalized)
    : _data(data), _itemSize(itemSize), _usage(usage), _dataType(dataType),
      _normalized(normalized) {
  _count = _data.size() / (_itemSize * getDataTypeSize());
  addUpdateRange(0, _count);
}

Attribute::Attribute(const std::vector<glm::vec2> &data,
                     const gl::BUFFER_USAGE &usage, const bool &normalized)
    : Attribute({}, 2, usage, gl::DATA_TYPE::FLOAT, normalized) {
  _data.resize(data.size() * sizeof(float) * 2);
  float *raw = (float *)_data.data();
  auto index = 0;
  for (auto i = 0; i < data.size(); i++) {
    raw[index++] = (data[i][0]);
    raw[index++] = (data[i][1]);
  }
}

Attribute::Attribute(const std::vector<glm::vec3> &data,
                     const gl::BUFFER_USAGE &usage, const bool &normalized)
    : Attribute({}, 3, usage, gl::DATA_TYPE::FLOAT, normalized) {
  _data.resize(data.size() * sizeof(float) * 3);
  float *raw = (float *)_data.data();
  auto index = 0;
  for (auto i = 0; i < data.size(); i++) {
    raw[index++] = (data[i][0]);
    raw[index++] = (data[i][1]);
    raw[index++] = (data[i][2]);
  }
}

Attribute::Attribute(const std::vector<glm::vec4> &data,
                     const gl::BUFFER_USAGE &usage, const bool &normalized)
    : Attribute({}, 4, usage, gl::DATA_TYPE::FLOAT, normalized) {
  _data.resize(data.size() * sizeof(float) * 4);
  float *raw = (float *)_data.data();
  auto index = 0;
  for (auto i = 0; i < data.size(); i++) {
    raw[index++] = (data[i][0]);
    raw[index++] = (data[i][1]);
    raw[index++] = (data[i][2]);
    raw[index++] = (data[i][3]);
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
  setVersion(getVersion() + 1);
}

const uint32_t Attribute::getDataTypeSize() const {
  switch (_dataType) {
  case gl::DATA_TYPE::BTYE:
    return sizeof(int8_t);
  case gl::DATA_TYPE::UNSIGNED_BYTE:
    return sizeof(uint8_t);
  case gl::DATA_TYPE::SHORT:
    return sizeof(int16_t);
  case gl::DATA_TYPE::UNSIGNED_SHORT:
    return sizeof(uint16_t);
  case gl::DATA_TYPE::INT:
    return sizeof(int32_t);
  case gl::DATA_TYPE::UNSIGNED_INT:
    return sizeof(uint32_t);
  case gl::DATA_TYPE::HALF_FLOAT:
    return sizeof(float) / 2;
  case gl::DATA_TYPE::FLOAT:
    return sizeof(float);
  case gl::DATA_TYPE::DOUBLE:
    return sizeof(double);
  case gl::DATA_TYPE::FIXED:
    return sizeof(float) * 2;
  case gl::DATA_TYPE::INT_2_10_10_10_REV:
    return sizeof(int32_t);
  case gl::DATA_TYPE::UNSIGNED_INT_2_10_10_10_REV:
    return sizeof(uint32_t);
  case gl::DATA_TYPE::UNSIGNED_INT_10F_11F_11F_REV:
    return sizeof(uint32_t);
  }
  return 0;
}
const uint32_t &Attribute::getItemSize() const { return _itemSize; }
const bool &Attribute::isNormalized() const { return _normalized; }
const gl::BUFFER_USAGE &Attribute::getUsage() const { return _usage; }
const gl::DATA_TYPE &Attribute::getDataType() const { return _dataType; }
const std::vector<char> &Attribute::getData() const { return _data; }
std::vector<char> &Attribute::getData() { return _data; }

void Attribute::write(const uint32_t &offset, const void *data,
                      const uint32_t &count) {
  auto start = offset * _itemSize * getDataTypeSize();
  auto size = count * _itemSize * getDataTypeSize();
  if (start + size > _data.size()) {
    _data.resize(start + size);
  }
  for (auto i = 0; i < size; i++) {
    _data[i + start] = ((char *)data)[i];
  }
  addUpdateRange(offset, count);
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