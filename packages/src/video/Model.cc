#include "video/Model.hpp"
#include "core/AutoPtr.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"

using namespace firefly;
using namespace firefly::video;
Model::Model(core::AutoPtr<Geometry> geometry, core::AutoPtr<Material> material)
    : _geometry(geometry), _material(material) {}

core::AutoPtr<Geometry> Model::getGeometry() { return _geometry; }

const core::AutoPtr<Geometry> &Model::getGeometry() const { return _geometry; }

core::AutoPtr<Material> Model::getMaterial() { return _material; }

const core::AutoPtr<Material> &Model::getMaterial() const { return _material; }