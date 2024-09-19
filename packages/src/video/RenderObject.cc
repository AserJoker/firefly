#include "video/RenderObject.hpp"
using namespace firefly;
using namespace firefly::video;

RenderObject::RenderObject(const core::AutoPtr<Geometry> &geometry,
                           const core::AutoPtr<Material> &material,
                           const glm::mat4 &matrixModel)
    : _geometry(geometry), _material(material), _matrixModel(matrixModel) {}
const core::AutoPtr<Geometry> &RenderObject::getGeometry() const {
  return _geometry;
}
const core::AutoPtr<Material> &RenderObject::getMeterial() const {
  return _material;
}
const glm::mat4 &RenderObject::getModelMatrix() const { return _matrixModel; }