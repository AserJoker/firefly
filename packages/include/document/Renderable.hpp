#pragma once
#include "core/AutoPtr.hpp"
#include "document/Node.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"
namespace firefly::document {
class Renderable : public Node {
protected:
  virtual const core::AutoPtr<video::Geometry> &getGeometry() const = 0;
  virtual const core::AutoPtr<video::Material> &getMaterial() const = 0;
  virtual const glm::mat4 &getMatrix() const = 0;
  void onTick() override;
};
}; // namespace firefly::document