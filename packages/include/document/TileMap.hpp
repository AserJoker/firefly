#pragma once
#include "core/AutoPtr.hpp"
#include "document/Renderable.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"
namespace firefly::document {
class TileMap : public Renderable {
private:
  core::AutoPtr<video::Geometry> _geometry;
  core::AutoPtr<video::Material> _material;
  glm::mat4 _matrix;

protected:
  const core::AutoPtr<video::Geometry> &getGeometry() const override;
  const core::AutoPtr<video::Material> &getMaterial() const override;
  const glm::mat4 &getMatrix() const override;

public:
  TileMap();
};
}; // namespace firefly::document