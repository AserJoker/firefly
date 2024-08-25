#pragma once
#include "AttributeIndex.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "video/Attribute.hpp"
#include <unordered_map>
#include <vector>
namespace firefly::video {
using GeometryAttributes =
    std::unordered_map<uint32_t, core::AutoPtr<Attribute>>;
using GeometryRenderRange = std::pair<uint32_t, uint32_t>;

class Geometry : public core::Object {
private:
  GeometryAttributes _attributes;
  core::AutoPtr<AttributeIndex> _indices;
  std::vector<GeometryRenderRange> _renderRanges;

public:
  Geometry();
  const core::AutoPtr<AttributeIndex> &getIndices() const;
  core::AutoPtr<AttributeIndex> &getIndices();

  const GeometryAttributes &getAttributes() const;
  GeometryAttributes &getAttributes();

  const core::AutoPtr<Attribute> getAttribute(const uint32_t &name) const;

  core::AutoPtr<Attribute> getAttribute(const uint32_t &name);

  void setAttribute(const uint32_t &name,
                    const core::AutoPtr<Attribute> &attribute);

  const bool hasAttribute(const uint32_t &name) const;

  void removeAttribute(const uint32_t &name);

  core::AutoPtr<Geometry> clone();

  const std::vector<GeometryRenderRange> &getRenderRanges() const;

  void setRenderRange(const uint32_t &start, const uint32_t &count);
};
} // namespace firefly::video