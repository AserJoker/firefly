#pragma once
#include "AttributeIndex.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "video/Attribute.hpp"
#include <unordered_map>
#include <vector>
namespace firefly::video {
using GeometryAttributes =
    std::unordered_map<std::string, core::AutoPtr<Attribute>>;
using GeometryRenderRange = std::pair<uint32_t, uint32_t>;

class Geometry : public core::Object {
private:
  GeometryAttributes _attributes;
  core::AutoPtr<AttributeIndex> _indices;
  std::vector<GeometryRenderRange> _renderRanges;
  bool _needUpdate;

public:
  Geometry();
  const core::AutoPtr<AttributeIndex> &getIndices() const;
  core::AutoPtr<AttributeIndex> &getIndices();

  const GeometryAttributes &getAttributes() const;
  GeometryAttributes &getAttributes();

  const core::AutoPtr<Attribute> getAttribute(const std::string &name) const;

  core::AutoPtr<Attribute> getAttribute(const std::string &name);

  void setAttribute(const std::string &name,
                    const core::AutoPtr<Attribute> &attribute);

  const bool hasAttribute(const std::string &name) const;

  void removeAttribute(const std::string &name);

  core::AutoPtr<Geometry> clone();

  const std::vector<GeometryRenderRange> &getRenderRanges() const;

  void setRenderRange(const uint32_t &start, const uint32_t &count);

  const bool isNeedUpdate() const;
  void setNeedUpdate(const bool& value);
};
} // namespace firefly::video