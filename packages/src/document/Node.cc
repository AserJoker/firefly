#include "document/Node.hpp"
#include "core/AutoPtr.hpp"
#include "core/Value.hpp"

using namespace firefly;
using namespace firefly::document;

core::Map<std::string, Node *> Node::_indexedNodes;

core::AutoPtr<Node> Node::query(const std::string &id) {
  if (_indexedNodes.contains(id)) {
    return _indexedNodes.at(id);
  }
  return nullptr;
}

Node::Node() : _parent(nullptr) {}
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
      item.node->_bindingHosts.erase(this);
    }
  }
  _bindingHosts.clear();
  _children.clear();
  if (!_id.empty()) {
    _indexedNodes.erase(_id);
  }
}
void Node::setId(const std::string &id) {
  if (!_id.empty()) {
    _indexedNodes.erase(_id);
  }
  _id = id;
  _indexedNodes[_id] = this;
}

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
}
void Node::onAttrChange(const std::string &name) {
  if (!_bindings.contains(name)) {
    return;
  }
  auto &bindings = _bindings.at(name);
  for (auto &binding : bindings) {
    if (_attributes.contains(name)) {
      binding.node->setAttribute(binding.attr, _attributes[name]);
      continue;
    }
    auto &group = _attributeGroups[name];
    for (auto &item : group) {
      auto fieldName = item.substr(name.length());
      binding.node->setAttribute(binding.attr + fieldName, _attributes[item]);
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
  }
}

std::list<core::AutoPtr<Node>> &Node::getChildren() { return _children; }

void Node::onTick() {
  for (auto &c : _children) {
    c->onTick();
  }
}

bool Node::setAttribute(const std::string &name, const core::ValuePtr &value) {
  auto currentName = name;
  if (!_attrGroup.empty()) {
    currentName = _attrGroup + "." + name;
  }
  if (!_attributes.contains(currentName)) {
    return false;
  }
  auto &current = _attributes.at(currentName);
  current = value.toValue();
  if (_attrGroup.empty()) {
    onAttrChange(currentName);
  }
  return true;
}

bool Node::setAttribute(const std::string &name, const core::Value &value) {
  auto currentName = name;
  if (!_attrGroup.empty()) {
    currentName = _attrGroup + "." + name;
  }
  if (!_attributes.contains(currentName)) {
    return false;
  }
  auto &current = _attributes.at(currentName);
  current = value;
  if (_attrGroup.empty()) {
    onAttrChange(currentName);
  }
  return true;
}
const core::Value Node::getAttribute(const std::string &name) const {
  if (!_attributes.contains(name)) {
    return {};
  }
  auto &attr = _attributes.at(name);
  return attr.toValue();
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

  auto it = _bindingHosts.find((Node *)host.getRawPointer());
  if (it != _bindingHosts.end()) {
    _bindingHosts.pushBack((Node *)host.getRawPointer());
  }
  auto &group = host->_bindings[source];
  group.pushBack({name, this});
}
const core::Map<std::string, std::string> Node::getAttributes() const {
  core::Map<std::string, std::string> attributes;
  for (auto &[name, attr] : _attributes) {
    attributes[name] = attr.getTypeName();
  }
  return attributes;
}
const core::Map<std::string, core::Array<std::string>> &
Node::getAttributeGroups() const {
  return _attributeGroups;
}