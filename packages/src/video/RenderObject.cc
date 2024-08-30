#include "video/RenderObject.hpp"
using namespace firefly;
using namespace firefly::video;

RenderObject::RenderObject(const core::AutoPtr<Geometry> &geometry,
                           const core::AutoPtr<Material> &material)
    : _geometry(geometry), _material(material) {}
const core::AutoPtr<Geometry> &RenderObject::getGeometry() const {
  return _geometry;
}
const core::AutoPtr<Material> &RenderObject::getMeterial() const {
  return _material;
}