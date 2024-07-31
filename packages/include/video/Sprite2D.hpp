#pragma once
#include "Geometry.hpp"
#include "RenderObject.hpp"
#include "Texture2D.hpp"
#include "core/AutoPtr.hpp"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
namespace firefly::video {
class Sprite2D : public RenderObject {
private:
  core::AutoPtr<Texture2D> _texture;
  Geometry::Rect _source;
  Geometry::Rect _target;
  glm::mat4 _model;
  glm::mat3 _uv_model;
  float _zIndex;

public:
  void draw() override;
  const Geometry::Rect &source() const;
  const Geometry::Rect &target() const;
  const float &zIndex() const;
  void setSource(const Geometry::Rect &rc);
  void setTarget(const Geometry::Rect &rc);
  void setZIndex(const float &z);
  void setTexture(const core::AutoPtr<Texture2D> &texture);
  const core::AutoPtr<Texture2D> &texture() const;
};
}; // namespace firefly::video