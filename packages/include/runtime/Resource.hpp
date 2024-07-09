#pragma once
#include "core/Object.hpp"
namespace firefly::runtime {
class Resource : public core::Object {
public:
  virtual void *read(const size_t &size = 0, size_t *len = 0) = 0;
  virtual void write(void *buf, const size_t &size) = 0;
};
} // namespace firefly::runtime