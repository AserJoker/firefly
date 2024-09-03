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
  Geometry(const std::string &name);
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
  static constexpr const uint32_t ATTR_TEXCOORD_0 = 3;
  static constexpr const uint32_t ATTR_TEXCOORD_1 = 4;
  static constexpr const uint32_t ATTR_TEXCOORD_2 = 5;
  static constexpr const uint32_t ATTR_TEXCOORD_3 = 6;
  static constexpr const uint32_t ATTR_TEXCOORD_4 = 7;
  static constexpr const uint32_t ATTR_TEXCOORD_5 = 8;
  static constexpr const uint32_t ATTR_TEXCOORD_6 = 9;
  static constexpr const uint32_t ATTR_TEXCOORD_7 = 10;
  static constexpr const uint32_t ATTR_CUSTOM = 11;
};
} // namespace firefly::video