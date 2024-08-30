#include "video/Scene.hpp"
#include "video/Object3D.hpp"
using namespace firefly;
using namespace firefly::video;
const core::AutoPtr<Object3D> &Scene::getRoot() const { return _root; }
core::AutoPtr<Object3D> &Scene::getRoot() { return _root; }
Scene::Scene() { _root = new Object3D(); }