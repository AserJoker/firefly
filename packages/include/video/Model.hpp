#pragma once
#include "Geometry.hpp"
#include "Material.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <unordered_map>
namespace firefly::video {
class Model : public core::Object, public core::Cache<Model> {
private:
  core::AutoPtr<Geometry> _geometry;
  core::AutoPtr<Material> _material;
  std::unordered_map<std::string, core::AutoPtr<Model>> _children;

public:
  Model(core::AutoPtr<Geometry> geometry, core::AutoPtr<Material> material);
  core::AutoPtr<Geometry> getGeometry();
  const core::AutoPtr<Geometry> &getGeometry() const;
  core::AutoPtr<Material> getMaterial();
  const core::AutoPtr<Material> &getMaterial() const;
  const std::unordered_map<std::string, core::AutoPtr<Model>> &
  getChildren() const;
  std::unordered_map<std::string, core::AutoPtr<Model>> &getChildren();

public:
  static std::unordered_map<std::string, core::AutoPtr<Model>>
  load(const std::string &name);
};
} // namespace firefly::video