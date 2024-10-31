#pragma once
#include "core/Object.hpp"
namespace firefly::core {
class Buffer : public Object {
private:
  core::Unsigned_t _size;
  void *_data;

public:
  Buffer(core::Unsigned_t size = 0, const void *data = 0);
  ~Buffer() override;
  const void *getData() const;
  const core::Unsigned_t &getSize() const;
  void setData(core::Unsigned_t offset, core::Unsigned_t length,
               const void *data);
};
} // namespace firefly::core