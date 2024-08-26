#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <utility>
#include <vector>
namespace firefly::video {
class AttributeIndex : public core::Object {
private:
  std::vector<uint32_t> _data;
  std::vector<std::pair<uint32_t, uint32_t>> _updateList;

  void addUpdateRange(const uint32_t &start, const uint32_t &count);

public:
  AttributeIndex();
  const std::vector<uint32_t> &getData() const;
  std::vector<uint32_t> &getData();
  void write(const uint32_t &offset, const std::vector<uint32_t> &data);
  const uint32_t &read(const uint32_t &offset);
  core::AutoPtr<AttributeIndex> clone() const;
  const std::vector<std::pair<uint32_t, uint32_t>> &getUpdateRangeList() const;
  void clearUpdateRangeList();
};
}; // namespace firefly::video