#pragma once
#include "Geometry.hpp"
#include "Material.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
namespace firefly::video {
class Model : public core::Object {
private:
  core::AutoPtr<Geometry> _geometry;
  core::AutoPtr<Material> _material;

public:
  Model(core::AutoPtr<Geometry> geometry, core::AutoPtr<Material> material);
  static std::vector<core::AutoPtr<Model>> load(const std::string &name);
  core::AutoPtr<Geometry> getGeometry();
  const core::AutoPtr<Geometry> &getGeometry() const;
  core::AutoPtr<Material> getMaterial();
  const core::AutoPtr<Material> &getMaterial() const;
};
} // namespace firefly::video