#pragma once
#include "Geometry.hpp"
#include "Material.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
namespace firefly::video {
class RenderObject : public core::Object {
private:
  core::AutoPtr<Geometry> _geometry;
  core::AutoPtr<Material> _material;

public:
  RenderObject(const core::AutoPtr<Geometry> &geometry,
               const core::AutoPtr<Material> &material);
  const core::AutoPtr<Geometry> &getGeometry() const;
  const core::AutoPtr<Material> &getMeterial() const;
};
} // namespace firefly::video