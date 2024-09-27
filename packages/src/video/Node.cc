#include "video/Node.hpp"
#include "core/AutoPtr.hpp"
using namespace firefly;
using namespace firefly::video;
Node::Node() : _parent(nullptr) {}
void Node::setId(const std::string &id) { _id = id; }

const std::string &Node::getId() const { return _id; }

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
  if (!child->getId().empty()) {
    _indexedChildren[child->getId()] = child;
  }
}

void Node::removeChild(core::AutoPtr<Node> child) {
  if (child->_parent != this) {
    return;
  }
  auto it = std::find(_children.begin(), _children.end(), child);
  if (it != _children.end()) {
    child->_parent = nullptr;
    _children.erase(it);
    if (!child->getId().empty()) {
      _indexedChildren.erase(child->getId());
    }
  }
}

std::list<core::AutoPtr<Node>> &Node::getChildren() { return _children; }

void Node::onTick() {
  for (auto &c : _children) {
    c->onTick();
  }
}

core::AutoPtr<Node> Node::getChild(const std::string &id) {
  if (_indexedChildren.contains(id)) {
    return _indexedChildren.at(id);
  }
  return nullptr;
}

const core::AutoPtr<Node> Node::getChild(const std::string &id) const {
  if (_indexedChildren.contains(id)) {
    return _indexedChildren.at(id);
  }
  return nullptr;
}