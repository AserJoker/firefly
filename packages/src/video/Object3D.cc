#include "video/Object3D.hpp"
using namespace firefly;
using namespace firefly::video;
Object3D::Object3D() : _parent(nullptr) {}
void Object3D::add(const core::AutoPtr<Object3D> &child) {
  auto c = child;
  if (child->_parent == this) {
    return;
  }
  if (c->_parent != nullptr) {
    c->_parent->remove(c);
  }
  c->_parent = this;
  _children.push_back(c);
}
void Object3D::remove(const core::AutoPtr<Object3D> &child) {
  auto c = child;
  auto it = std::find(_children.begin(), _children.end(), child);
  if (it != _children.end()) {
    _children.erase(it);
  }
  c->_parent = nullptr;
}
const std::vector<core::AutoPtr<Object3D>> &Object3D::getChildren() const {
  return _children;
}
const core::AutoPtr<Object3D> Object3D::getParent() const { return _parent; }
void Object3D::draw(core::AutoPtr<Renderer> renderer) {}