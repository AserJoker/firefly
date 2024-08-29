#pragma once
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/Object.hpp"
namespace firefly::video {
class AttributeIndex : public core::Object {
private:
  core::AutoPtr<core::Buffer> _buffer;

public:
  AttributeIndex();
  const uint32_t *getIndices() const;
  const uint32_t getIndicesCount() const;
  void write(const uint32_t &offset, const uint32_t &size,
             const uint32_t *data);
};
}; // namespace firefly::video