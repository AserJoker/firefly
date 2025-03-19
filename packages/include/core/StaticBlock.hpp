#pragma once
namespace firefly::core {
class StaticBlock {
public:
  template <class T> StaticBlock(T &&cb) { cb(); }
  StaticBlock() = default;
};
}; // namespace firefly::core