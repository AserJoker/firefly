#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "gl/BufferUsage.hpp"
#include "gl/DataType.hpp"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <utility>
#include <vector>
namespace firefly::video {
using AttributeRange = std::pair<uint32_t, uint32_t>;
using AttributeUpdateRangeList = std::vector<AttributeRange>;
class Attribute : public core::Object {
private:
  std::vector<char> _data;
  uint32_t _itemSize;
  gl::BUFFER_USAGE _usage;
  gl::DATA_TYPE _dataType;
  bool _normalized;
  uint32_t _count;

  AttributeUpdateRangeList _updateList;

  void addUpdateRange(const uint32_t &start, const uint32_t &count);

public:
  Attribute(const std::vector<char> &data, const uint32_t &itemSize,
            const gl::BUFFER_USAGE &usage, const gl::DATA_TYPE &dataType,
            const bool &normalized = false);

  Attribute(const std::vector<glm::vec2> &data, const gl::BUFFER_USAGE &usage,
            const bool &normalized = false);

  Attribute(const std::vector<glm::vec3> &data, const gl::BUFFER_USAGE &usage,
            const bool &normalized = false);

  Attribute(const std::vector<glm::vec4> &data, const gl::BUFFER_USAGE &usage,
            const bool &normalized = false);
  const uint32_t getDataTypeSize() const;
  const uint32_t &getItemSize() const;
  const bool &isNormalized() const;
  const gl::BUFFER_USAGE &getUsage() const;
  const gl::DATA_TYPE &getDataType() const;
  const std::vector<char> &getData() const;
  std::vector<char> &getData();

  void write(const uint32_t &index, const void *data, const uint32_t &size);
  void *read(const uint32_t &index);

  const AttributeUpdateRangeList &getUpdateRangeList() const;
  void clearUpdateRangeList();

  core::AutoPtr<Attribute> clone() const;
};
} // namespace firefly::video