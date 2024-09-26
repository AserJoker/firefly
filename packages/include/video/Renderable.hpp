#pragma once
#include "Geometry.hpp"
#include "core/AutoPtr.hpp"
#include "video/Material.hpp"
#include "video/Node.hpp"
#include <glm/glm.hpp>
namespace firefly::video {
class Renderable : public Node {
public:
  void onTick() override;

protected:
  virtual const core::AutoPtr<Geometry> &getGeometry() const = 0;
  virtual const core::AutoPtr<Material> &getMaterial() const = 0;
  virtual const glm::mat4 &getModelMatrix() const = 0;
};
} // namespace firefly::video