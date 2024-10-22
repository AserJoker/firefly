#include "document/Node.hpp"
using namespace firefly;
using namespace firefly::document;
core::Map<core::String_t, Node *> Node::_indexedNodes;

Node::Node() : _parent(nullptr), _ready(false) {}

Node::~Node() {
  while (!_children.empty()) {
    removeChild(*_children.begin());
  }
  if (!_id.empty()) {
    Node::_indexedNodes.erase(_id);
  }
}

const core::Array<core::AutoPtr<Node>> &Node::getChildren() const {
  return _children;
}

core::Array<core::AutoPtr<Node>> &Node::getChildren() { return _children; }

core::AutoPtr<Node> Node::getParent() { return _parent; }

const core::AutoPtr<Node> Node::getParent() const { return _parent; }

void Node::setIdentity(const core::String_t &id) {
  if (!_id.empty()) {
    Node::_indexedNodes.erase(_id);
  }
  _id = id;
  if (!_id.empty()) {
    Node::_indexedNodes[_id] = this;
  }
}

const core::String_t &Node::getIdentity() const { return _id; }

void Node::appendChild(core::AutoPtr<Node> child) {
  if (child->_parent == this) {
    return;
  }
  if (child->_parent && child->_parent != this) {
    child->_parent->removeChild(child);
  }
  child->_parent = this;
  _children.pushBack(child);
}

void Node::removeChild(core::AutoPtr<Node> child) {
  if (child->_parent == this) {
    child->onUnload();
    _children.erase(child);
    child->_parent = nullptr;
  }
}

void Node::onTick() {
  if (!_ready) {
    _ready = true;
    onLoad();
  }
  for (auto &child : _children) {
    child->onTick();
  }
}

core::AutoPtr<Node> Node::select(const core::String_t &identity) {
  if (!Node::_indexedNodes.contains(identity)) {
    return nullptr;
  }
  return _indexedNodes.at(identity);
}

void Node::onLoad() {}

void Node::onUnload() {
  _ready = false;
  for (auto &child : _children) {
    child->onUnload();
  }
}