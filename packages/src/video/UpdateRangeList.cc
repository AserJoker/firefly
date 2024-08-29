#include "video/UpdateRangeList.hpp"
using namespace firefly;
using namespace firefly::video;

void UpdateRangeList::clear() { _ranges.clear(); }
void UpdateRangeList::add(const uint32_t &start, const uint32_t &size) {
  for (auto &[_start, _count] : _ranges) {
    if (start >= _start && start <= start + _count) {
      if (_count < start + size) {
        _count = start + size;
      }
      return;
    }
    if (start + size >= _start && start + size <= _start + _count) {
      if (_start > start) {
        _start = start;
      }
      return;
    }
  }
  _ranges.push_back({start, size});
}
const std::vector<std::pair<uint32_t, uint32_t>> &
UpdateRangeList::getRanges() const {
  return _ranges;
}