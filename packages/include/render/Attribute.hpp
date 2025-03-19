#pragma once
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/ObjectBase.hpp"
namespace firefly::render {
class Attribute : public core::ObjectBase {
private:
  core::AutoPtr<core::Buffer> _buffer;
  bool _normalize;
  uint32_t _itemCount;

public:
  Attribute(uint32_t itemCount,
            const core::AutoPtr<core::Buffer> &buffer = nullptr,
            bool normalize = false);

  Attribute(uint32_t itemCount, void *ptr, uint32_t size,
            bool normalize = false);

  ~Attribute() override = default;

  bool isNormalize() const;

  void setIsNormalize(bool value);

  uint32_t getItemCount() const;

  void setItemCount(uint32_t count);

  const core::AutoPtr<core::Buffer> &getBuffer() const;

  void setBuffer(const core::AutoPtr<core::Buffer> &buffer);
};
}; // namespace firefly::render