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
bool Node::setAttribute(const std::string &name, const Attr &value) {
  auto currentName = name;
  if (!_attrGroup.empty()) {
    currentName = _attrGroup + "." + name;
  }
  if (!_attributes.contains(currentName)) {
    return false;
  }
  auto &current = _attributes.at(currentName);
  if (current.type != value.type) {
    return false;
  }
  switch (current.type) {
  case BOOLEAN:
    *current.boolean = *value.boolean;
    break;
  case STRING:
    *current.string = *value.string;
    break;
  case I32:
    *current.i32 = *value.i32;
    break;
  case U32:
    *current.u32 = *value.u32;
    break;
  case F32:
    *current.f32 = *value.f32;
    break;
  case F64:
    *current.f64 = *value.f64;
    break;
  case NIL:
    break;
  }
  if (_attrGroup.empty()) {
    onAttrChange(currentName);
  }
  return true;
}

bool Node::setAttribute(const std::string &name, const AttrValue &value) {
  auto currentName = name;
  if (!_attrGroup.empty()) {
    currentName = _attrGroup + "." + name;
  }
  if (!_attributes.contains(currentName)) {
    return false;
  }
  auto &current = _attributes.at(currentName);
  switch (current.type) {
  case BOOLEAN:
    *current.boolean = value.toBoolean();
    break;
  case STRING:
    *current.string = value.toString();
    break;
  case I32:
    *current.i32 = value.type == I32 ? value.i32 : (int32_t)value.toNumber();
    break;
  case U32:
    *current.u32 = value.type == U32 ? value.u32 : (uint32_t)value.toNumber();
    break;
  case F32:
    *current.f32 = value.type == F32 ? value.f32 : (float)value.toNumber();
    break;
  case F64:
    *current.f64 = value.toNumber();
    break;
  case NIL:
    break;
  }
  if (_attrGroup.empty()) {
    onAttrChange(currentName);
  }
  return true;
}
const Node::AttrValue Node::getAttribute(const std::string &name) const {
  if (!_attributes.contains(name)) {
    return {};
  }
  auto &attr = _attributes.at(name);
  switch (attr.type) {
  case BOOLEAN:
    return *attr.boolean;
  case STRING:
    return *attr.string;
  case I32:
    return *attr.i32;
  case U32:
    return *attr.u32;
  case F32:
    return *attr.f32;
  case F64:
    return *attr.f64;
  case NIL:
    return {};
  }
  return {};
}
void Node::beginAttrGroup(const std::string &name) {
  if (!_attrGroup.empty()) {
    _attrGroups.push_back(_attrGroup);
    _attrGroup += "." + name;
  } else {
    _attrGroup = name;
  }
}
void Node::endAttrGroup() {
  if (!_attrGroup.empty()) {
    onAttrChange(_attrGroup);
    _attrGroup.clear();
  } else {
    _attrGroup = *_attrGroups.rbegin();
    _attrGroups.pop_back();
  }
}