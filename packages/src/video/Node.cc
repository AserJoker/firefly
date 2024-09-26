#include "video/Node.hpp"
#include "core/AutoPtr.hpp"
using namespace firefly;
using namespace firefly::video;
Node::Node() : _parent(nullptr) {}
core::AutoPtr<Node> Node::getParent() { return _parent; }
void Node::appendChild(core::AutoPtr<Node> child) {
  if (child->_parent == this) {
    return;
  }
  _children.push_back(child);
  if (child->_parent) {
    child->_parent->removeChild(child);
  }
  child->_parent = this;
}
void Node::removeChild(core::AutoPtr<Node> child) {
  if (child->_parent != this) {
    return;
  }
  auto it = std::find(_children.begin(), _children.end(), child);
  if (it != _children.end()) {
    child->_parent = nullptr;
    _children.erase(it);
  }
}

std::list<core::AutoPtr<Node>> &Node::getChildren() { return _children; }
void Node::onTick() {
  for (auto &c : _children) {
    c->onTick();
  }
}