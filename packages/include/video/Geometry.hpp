#pragma once
#include "core/Array.hpp"
#include "core/AutoPtr.hpp"
#include "core/Cache.hpp"
#include "core/Map.hpp"
#include "core/Object.hpp"
#include "gl/DrawMode.hpp"
#include "gl/VertexArray.hpp"
#include "video/Attribute.hpp"
#include "video/AttributeIndex.hpp"
#include <glm/glm.hpp>


namespace firefly::video {
class Geometry : public core::Object, public core::Cache<Geometry> {

private:
  core::Map<core::Unsigned_t, core::AutoPtr<Attribute>> _attributes;
  core::AutoPtr<AttributeIndex> _attrIndex;
  core::AutoPtr<gl::VertexArray> _vao;

public:
  Geometry();
  Geometry(const core::Array<core::AutoPtr<Attribute>> &attributes,
           const core::AutoPtr<AttributeIndex> &index);

  void setAttribute(core::Unsigned_t index,
                    const core::AutoPtr<Attribute> &attribute);

  core::AutoPtr<Attribute> getAttribute(core::Unsigned_t index);
  const core::AutoPtr<Attribute> getAttribute(core::Unsigned_t index) const;
  void setVertexAttribDivisor(core::Unsigned_t index,
                              core::Unsigned_t divisor = 1);
  void removeAttribute(core::Unsigned_t index);
  const core::Map<core::Unsigned_t, core::AutoPtr<Attribute>> &
  getAttributes() const;

  void setAttributeIndex(const core::AutoPtr<AttributeIndex> &attrindex);

  const core::AutoPtr<AttributeIndex> &getAttributeIndex() const;
  core::AutoPtr<AttributeIndex> &getAttributeIndex();
  virtual void draw(gl::DRAW_MODE mode) const;
  virtual void drawInstanced(gl::DRAW_MODE mode, core::Unsigned_t count) const;

public:
  static constexpr const core::Unsigned_t ATTR_POSITION = 0;
  static constexpr const core::Unsigned_t ATTR_NORMAL = 1;
  static constexpr const core::Unsigned_t ATTR_COLOR = 2;
  static constexpr const core::Unsigned_t ATTR_TEXCOORD = 3;
};
} // namespace firefly::video