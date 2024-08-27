#pragma once
#include "video/Material.hpp"
#include "video/Renderer.hpp"
#include "core/AutoPtr.hpp"
#include "video/Geometry.hpp"
#include "video/Object3D.hpp"
namespace firefly::video {
class Mesh : public Object3D {
private:
  core::AutoPtr<Geometry> _geometry;
  core::AutoPtr<Material> _material;

public:
  Mesh(const core::AutoPtr<Geometry> &geometry = new Geometry(),
       const core::AutoPtr<Material> &material = 0);
  core::AutoPtr<Geometry> getGeometry();
  core::AutoPtr<Material> getMaterial();
  void draw(core::AutoPtr<Renderer> renderer) override;
};
} // namespace firefly::video