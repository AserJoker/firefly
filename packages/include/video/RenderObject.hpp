#pragma once
#include "Geometry.hpp"
#include "Material.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <glm/glm.hpp>
namespace firefly::video {
class RenderObject : public core::Object {
private:
  core::AutoPtr<Geometry> _geometry;
  core::AutoPtr<Material> _material;
  glm::mat4 _matrixModel;

public:
  RenderObject(const core::AutoPtr<Geometry> &geometry,
               const core::AutoPtr<Material> &material,
               const glm::mat4 &matrixModel);
  const core::AutoPtr<Geometry> &getGeometry() const;
  const core::AutoPtr<Material> &getMeterial() const;
  const glm::mat4 &getModelMatrix() const;
};
} // namespace firefly::video