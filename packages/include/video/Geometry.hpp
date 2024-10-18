#pragma once
#include "core/AutoPtr.hpp"
#include "core/Cache.hpp"
#include "core/Object.hpp"
#include "gl/DrawMode.hpp"
#include "gl/VertexArray.hpp"
#include "video/Attribute.hpp"
#include "video/AttributeIndex.hpp"
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
namespace firefly::video {
class Geometry : public core::Object, public core::Cache<Geometry> {

private:
  std::unordered_map<uint32_t, core::AutoPtr<Attribute>> _attributes;
  core::AutoPtr<AttributeIndex> _attrIndex;
  core::AutoPtr<gl::VertexArray> _vao;

public:
  Geometry();
  Geometry(const std::vector<core::AutoPtr<Attribute>> &attributes,
           const core::AutoPtr<AttributeIndex> &index);

  void setAttribute(uint32_t index, const core::AutoPtr<Attribute> &attribute);

  core::AutoPtr<Attribute> getAttribute(uint32_t index);
  const core::AutoPtr<Attribute> getAttribute(uint32_t index) const;
  void setVertexAttribDivisor(uint32_t index, uint32_t divisor = 1);
  void removeAttribute(uint32_t index);
  const std::unordered_map<uint32_t, core::AutoPtr<Attribute>> &
  getAttributes() const;

  void setAttributeIndex(const core::AutoPtr<AttributeIndex> &attrindex);

  const core::AutoPtr<AttributeIndex> &getAttributeIndex() const;
  core::AutoPtr<AttributeIndex> &getAttributeIndex();
  virtual void draw(gl::DRAW_MODE mode) const;
  virtual void drawInstanced(gl::DRAW_MODE mode, uint32_t count) const;

public:
  static constexpr const uint32_t ATTR_POSITION = 0;
  static constexpr const uint32_t ATTR_NORMAL = 1;
  static constexpr const uint32_t ATTR_COLOR = 2;
  static constexpr const uint32_t ATTR_TEXCOORD = 3;
};
} // namespace firefly::video