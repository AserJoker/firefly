#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "video/Attribute.hpp"
#include "video/AttributeIndex.hpp"
#include <unordered_map>
namespace firefly::video {
class Geometry : public core::Object {
private:
  std::unordered_map<uint32_t, core::AutoPtr<Attribute>> _attributes;
  core::AutoPtr<AttributeIndex> _indices;

public:
  Geometry();
  void setAttribute(const uint32_t &index,
                    const core::AutoPtr<Attribute> &attribute);
  core::AutoPtr<Attribute> getAttribute(const uint32_t &index);
  const core::AutoPtr<Attribute> getAttribute(const uint32_t &index) const;
  void removeAttribute(const uint32_t &index);
  const std::unordered_map<uint32_t, core::AutoPtr<Attribute>> &
  getAttributes() const;
  const core::AutoPtr<AttributeIndex> &getIndices() const;
  core::AutoPtr<AttributeIndex> &getIndices();

public:
  static constexpr const uint32_t ATTR_POSITION = 0;
  static constexpr const uint32_t ATTR_NORMAL = 1;
  static constexpr const uint32_t ATTR_COLOR = 2;
  static constexpr const uint32_t ATTR_TEXCOORD = 3;
  static constexpr const uint32_t ATTR_CUSTOM = 4;
};
} // namespace firefly::video