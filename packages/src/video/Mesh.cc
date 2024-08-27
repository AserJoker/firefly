#include "video/Mesh.hpp"
#include "core/AutoPtr.hpp"
#include "video/Geometry.hpp"
using namespace firefly;
using namespace firefly::video;
Mesh::Mesh(const core::AutoPtr<Geometry> &geometry,
           const core::AutoPtr<Material> &material)
    : _geometry(geometry), _material(material) {}
core::AutoPtr<Geometry> Mesh::getGeometry() { return _geometry; }
core::AutoPtr<Material> Mesh::getMaterial() { return _material; }
void Mesh::draw(core::AutoPtr<Renderer> renderer) {
  renderer->setMaterial(_material);
  renderer->renderGeometry(_geometry);
}