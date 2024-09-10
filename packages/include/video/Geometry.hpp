#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "video/Attribute.hpp"
#include "video/AttributeIndex.hpp"
#include <glm/glm.hpp>
#include <unordered_map>
namespace firefly::video {
class Geometry : public core::Object {
public:
  struct Bounding {
    float left, right, top, bottom, front, back;
  };

  struct BoundingShpere {
    glm::vec3 center;
    float radis;
  };

private:
  std::unordered_map<uint32_t, core::AutoPtr<Attribute>> _attributes;
  core::AutoPtr<AttributeIndex> _indices;
  Bounding _binding;
  BoundingShpere _bindingShpere;

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
  void computeBounding();
  void computeBoundingSphere();
  const Bounding &getBounding() const;
  const BoundingShpere &getBoundingSphere() const;

public:
  static constexpr const uint32_t ATTR_POSITION = 0;
  static constexpr const uint32_t ATTR_NORMAL = 1;
  static constexpr const uint32_t ATTR_COLOR = 2;
  static constexpr const uint32_t ATTR_TEXCOORD = 3;
  static constexpr const uint32_t ATTR_CUSTOM = 11;
};
} // namespace firefly::video