#pragma once
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/Object.hpp"
namespace firefly::runtime {
class Resource : public core::Object {
public:
  virtual core::AutoPtr<core::Buffer> read(const uint32_t &size = 0) = 0;
  virtual void write(const core::AutoPtr<core::Buffer> &data) = 0;
};
} // namespace firefly::runtime