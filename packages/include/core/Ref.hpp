#pragma once
#include <cstdint>
#include <unordered_map>
namespace firefly::core {
struct Ref {
private:
  static inline std::unordered_map<const void *, uint32_t> _refs;

public:
  static uint32_t addRef(const void *instance);
  static uint32_t dispose(const void *instance);
  static const uint32_t &ref(const void *instance);
};
}; // namespace firefly::core