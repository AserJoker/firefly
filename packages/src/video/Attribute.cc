#include "video/Attribute.hpp"
#include "video/UpdateRangeList.hpp"
using namespace firefly;
using namespace firefly::video;

Attribute::Attribute(const core::AutoPtr<core::Buffer> &buffer,
                     const std::type_info &type, const uint32_t &itemSize,
                     const bool &normalize, const bool &dynamic)
    : _buffer(buffer), _itemType(type.name()), _itemSize(itemSize),
      _normalize(normalize), _dynamic(dynamic) {
  if (type == typeid(float)) {
    _stride = sizeof(float) * itemSize;
  } else if (type == typeid(int32_t)) {
    _stride = sizeof(int32_t) * itemSize;
  } else if (type == typeid(uint32_t)) {
    _stride = sizeof(uint32_t) * itemSize;
  } else if (type == typeid(double)) {
    _stride = sizeof(double) * itemSize;
  } else if (type == typeid(int8_t)) {
    _stride = sizeof(int8_t) * itemSize;
  } else if (type == typeid(uint8_t)) {
    _stride = sizeof(uint8_t) * itemSize;
  } else if (type == typeid(int16_t)) {
    _stride = sizeof(int16_t) * itemSize;
  } else if (type == typeid(uint16_t)) {
    _stride = sizeof(uint16_t) * itemSize;
  }
  core::AutoPtr updateRangeList = new UpdateRangeList();
  updateRangeList->add(0, _buffer->getSize());
  setMetadata("video::update_range_list", updateRangeList);
}

const std::string &Attribute::getItemType() const { return _itemType; }

const uint32_t &Attribute::getItemSize() const { return _itemSize; }

const bool &Attribute::isNormalized() const { return _normalize; }

const bool &Attribute::isDynamic() const { return _dynamic; }

const core::AutoPtr<core::Buffer> &Attribute::getBuffer() const {
  return _buffer;
}
const uint32_t &Attribute::getStride() const { return _stride; }

void Attribute::write(const uint32_t &offset, const uint32_t &size,
                      const void *data) {
  _buffer->setData(offset, size, data);

  auto updateRangeList =
      getMetadata("video::update_range_list").cast<UpdateRangeList>();
  updateRangeList->add(offset, size);
  setVersion(getVersion() + 1);
}