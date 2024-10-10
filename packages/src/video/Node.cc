#include "video/Node.hpp"
#include "core/AutoPtr.hpp"
using namespace firefly;
using namespace firefly::video;
Node::Node() : _parent(nullptr), _changed(0) {}
Node::~Node() {
  for (auto &host : _bindingHosts) {
    for (auto &[_, group] : host->_bindings) {
      for (auto it = group.begin(); it != group.end(); it++) {
        if (it->node == this) {
          group.erase(it);
          break;
        }
      }
    }
  }
  for (auto &[_, binding] : _bindings) {
    for (auto &item : binding) {
      auto it = std::find(item.node->_bindingHosts.begin(),
                          item.node->_bindingHosts.end(), this);
      if (it != item.node->_bindingHosts.end()) {
        item.node->_bindingHosts.erase(it);
      }
    }
  }
  _bindingHosts.clear();
}
void Node::setId(const std::string &id) { _id = id; }

const std::string &Node::getId() const { return _id; }

core::AutoPtr<Node> Node::getParent() { return _parent; }

void Node::appendChild(core::AutoPtr<Node> child) {
  if (child == nullptr) {
    return;
  }
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
void Node::onAttrChange(const std::string &name) {
  if (_bindings.contains(name)) {
    auto &bindings = _bindings.at(name);
    for (auto &binding : bindings) {
      if (_attributes.contains(name)) {
        binding.node->setAttribute(binding.attr, _attributes[name]);
      } else {
        auto &group = _attributeGroups[name];
        for (auto &item : group) {
          auto fieldName = item.substr(name.length());
          binding.node->setAttribute(binding.attr + fieldName,
                                     _attributes[item]);
        }
      }
    }
  }
}

void Node::removeChild(core::AutoPtr<Node> child) {
  if (child == nullptr) {
    return;
  }
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
  AttrValue val(value);
  switch (current.type) {
  case BOOLEAN:
    *current.boolean = val.toBoolean();
    break;
  case STRING:
    *current.string = val.toString();
    break;
  case I32:
    *current.i32 = val.type == I32 ? val.i32 : (int32_t)val.toNumber();
    break;
  case U32:
    *current.u32 = val.type == U32 ? val.u32 : (uint32_t)val.toNumber();
    break;
  case F32:
    *current.f32 = val.type == F32 ? val.f32 : (float)val.toNumber();
    break;
  case F64:
    *current.f64 = val.type == F64 ? val.f64 : val.toNumber();
    break;
  case NIL:
    break;
  }
  if (_attrGroup.empty()) {
    onAttrChange(currentName);
  }
  _changed++;
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
  _changed++;
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
  if (_attrGroup.empty()) {
    return;
  }
  if (!_attrGroup.empty()) {
    onAttrChange(_attrGroup);
    _attrGroup.clear();
  } else {
    _attrGroup = *_attrGroups.rbegin();
    _attrGroups.pop_back();
  }
}
void Node::bindAttribute(const std::string name, core::AutoPtr<Node> host,
                         const std::string &source) {

  auto it = std::find(_bindingHosts.begin(), _bindingHosts.end(),
                      host.getRawPointer());
  if (it != _bindingHosts.end()) {
    _bindingHosts.push_back((Node *)host.getRawPointer());
  }
  auto &group = host->_bindings[source];
  group.push_back({name, this});
}