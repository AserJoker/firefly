#include "video/Object3D.hpp"
#include <algorithm>
using namespace firefly;
using namespace firefly::video;
Object3D::Object3D() : _visible(true) {}
void Object3D::add(const core::AutoPtr<Object3D> &child) {
  _children.push_back(child);
}
void Object3D::remove(const core::AutoPtr<Object3D> &child) {
  auto it = std::find(_children.begin(), _children.end(), child);
  if (it != _children.end()) {
    _children.erase(it);
  }
}
core::AutoPtr<Object3D> Object3D::getParent() { return _parent; }
const std::vector<core::AutoPtr<Object3D>> &Object3D::getChildren() const {
  return _children;
}
std::vector<core::AutoPtr<Object3D>> &Object3D::getChildren() {
  return _children;
}
void Object3D::setVisible(bool value) { _visible = value; }
const bool &Object3D::isVisible() const { return _visible; }