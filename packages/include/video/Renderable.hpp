#pragma once
#include "Geometry.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "video/Material.hpp"
#include <glm/glm.hpp>
#include <list>
namespace firefly::video {
class Renderable : public core::Object {
private:
  friend class Renderer;
  static std::list<Renderable *> _renderList;

public:
  Renderable();
  ~Renderable() override;

protected:
  virtual const core::AutoPtr<Geometry> &getGeometry() const = 0;
  virtual const core::AutoPtr<Material> &getMaterial() const = 0;
  virtual const glm::mat4 &getModelMatrix() const = 0;
};
} // namespace firefly::video