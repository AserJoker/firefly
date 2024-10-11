#pragma once
#include "video/Geometry.hpp"
#include "core/AutoPtr.hpp"
#include "video/Material.hpp"
#include "document/Node.hpp"
#include <glm/glm.hpp>
namespace firefly::document {
class Renderable : public Node {
public:
  void onTick() override;

protected:
  virtual const core::AutoPtr<video::Geometry> &getGeometry() const = 0;
  virtual const core::AutoPtr<video::Material> &getMaterial() const = 0;
  virtual const glm::mat4 &getMatrixModel() const = 0;
};
} // namespace firefly::video