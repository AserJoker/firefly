#include "document/Node.hpp"
#include "core/AutoPtr.hpp"
#include "core/Value.hpp"
#include <vector>

using namespace firefly;
using namespace firefly::document;
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
  _changed++;
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
  _changed++;
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

  auto it = std::find(_bindingHosts.begin(), _bindingHosts.end(),
                      host.getRawPointer());
  if (it != _bindingHosts.end()) {
    _bindingHosts.push_back((Node *)host.getRawPointer());
  }
  auto &group = host->_bindings[source];
  group.push_back({name, this});
}
const std::unordered_map<std::string, std::string> Node::getAttributes() const {
  std::unordered_map<std::string, std::string> attributes;
  for (auto &[name, attr] : _attributes) {
    attributes[name] = attr.getTypeName();
  }
  return attributes;
}
const std::unordered_map<std::string, std::vector<std::string>> &
Node::getAttributeGroups() const {
  return _attributeGroups;
}