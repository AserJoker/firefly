#pragma once
#include "core/Object.hpp"
#include <utility>
#include <vector>
namespace firefly::video {
class UpdateRangeList : public core::Object {
private:
  std::vector<std::pair<uint32_t, uint32_t>> _ranges;

public:
  void clear();
  void add(const uint32_t &start, const uint32_t &size);
  const std::vector<std::pair<uint32_t, uint32_t>> &getRanges() const;
};
}; // namespace firefly::video