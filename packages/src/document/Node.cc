#include "document/Node.hpp"
#include "core/AutoPtr.hpp"
#include "core/Json.hpp"
#include "core/Singleton.hpp"
#include "core/TypeException.hpp"
#include "core/Value.hpp"
#include "runtime/Media.hpp"
#include <exception>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlstring.h>

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

void Node::setName(const core::String_t &name) { _name = name; }

const core::String_t &Node::getName() const { return _name; }

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

void Node::beginPropGroup(const core::String_t &name) {
  if (_currentPropertyGroup.empty()) {
    _currentPropertyGroup = name;
  } else {
    if (_propertyGroupStack.empty()) {
      _propertyGroupStack.pushBack(_currentPropertyGroup);
      _currentPropertyGroup = name;
    } else {
      _propertyGroupStack.pushBack(_currentPropertyGroup);
      _currentPropertyGroup += "." + name;
    }
  }
}
void Node::endPropGroup() {
  if (_currentPropertyGroup.empty()) {
    return;
  }
  onPropChange(_currentPropertyGroup);
  if (_propertyGroupStack.empty()) {
    _currentPropertyGroup.clear();
  } else {
    _currentPropertyGroup = *_propertyGroupStack.rbegin();
    _propertyGroupStack.popBack();
  }
}

const core::Value Node::getProperty(const core::String_t &name) const {
  if (_properties.contains(name)) {
    auto value = _properties.at(name).get();
    if (value.is<core::String_t>()) {
      return value.as<core::String_t>();
    } else if (value.is<core::Integer_t>()) {
      return value.as<core::Integer_t>();
    } else if (value.is<core::Unsigned_t>()) {
      return value.as<core::Unsigned_t>();
    } else if (value.is<core::Float_t>()) {
      return value.as<core::Float_t>();
    } else if (value.is<core::Boolean_t>()) {
      return value.as<core::Boolean_t>();
    }
  }
  return nullptr;
}

void Node::setProperty(const core::String_t &name, const core::Value &value) {

  core::String_t attrName;
  if (!_currentPropertyGroup.empty()) {
    attrName = _currentPropertyGroup + "." + name;
  }
  if (!_properties.contains(attrName)) {
    attrName = name;
  }
  if (_propertyGroups.contains(attrName) &&
      value.getType() == core::ValueType::STRING) {
    core::String_t source = value.get<core::String_t>();
    setProperty(attrName, core::Json::parse(source));
    return;
  }
  if (_propertyGroups.contains(attrName) &&
      value.getType() == core::ValueType::MAP) {
    auto record = value.get<core::Map_t>();
    beginPropGroup(attrName);
    for (auto &[key, value] : record) {
      setProperty(key, value);
    }
    endPropGroup();
    return;
  }
  if (_properties.contains(attrName)) {
    try {
      auto &attr = _properties.at(attrName);
      core::Boolean_t isChanged = false;
      if (attr.type() == typeid(core::String_t)) {
        isChanged = attr.set(value.to<core::String_t>());
      } else if (attr.type() == typeid(core::Integer_t)) {
        isChanged = attr.set(value.to<core::Integer_t>());
      } else if (attr.type() == typeid(core::Unsigned_t)) {
        isChanged = attr.set(value.to<core::Unsigned_t>());
      } else if (attr.type() == typeid(core::Float_t)) {
        isChanged = attr.set(value.to<core::Float_t>());
      } else if (attr.type() == typeid(core::Boolean_t)) {
        isChanged = attr.set(value.to<core::Boolean_t>());
      } else {
        throw core::TypeException(
            fmt::format("unknown type '{}'", value.getTypeName()));
      }
      if (isChanged) {
        if (attrName == name) {
          onPropChange(name);
        }
      }
    } catch (std::exception &e) {
      throw core::TypeException(
          fmt::format("Failed to set attribute '{}'", attrName), e);
    }
  }
}
void Node::onMainLoop() {
  if (!_ready) {
    _ready = true;
    onLoad();
  }
  onTick();
}
void Node::onTick() {
  for (auto &child : _children) {
    child->onMainLoop();
  }
}

core::AutoPtr<Node> Node::create(const core::String_t &type) {
  if (_nodeConstructors.contains(type)) {
    return _nodeConstructors[type]();
  }
  return nullptr;
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

static core::AutoPtr<Node> parseXML(xmlNodePtr node) {
  auto name = core::String_t((core::CString_t)node->name);
  if (name == "text" || name == "comment") {
    return nullptr;
  }
  core::AutoPtr<Node> result = Node::create(name);
  if (!result) {
    result = new Node();
  }
  result->setName(name);
  auto prop = node->properties;
  while (prop != nullptr) {
    core::String_t key = (core::CString_t)prop->name;
    core::String_t value = (core::CString_t)xmlGetProp(node, prop->name);
    core::String_t name;
    for (auto &chr : key) {
      if (chr == '_') {
        name += ".";
      } else {
        name += chr;
      }
    }
    if (name == "id") {
      result->setIdentity(value);
    } else {
      result->setProperty(name, value);
    }
    prop = prop->next;
  }
  auto child = node->children;
  while (child != nullptr) {
    auto node = parseXML(child);
    if (node != nullptr) {
      result->appendChild(node);
    }
    child = child->next;
  }
  return result;
}

core::AutoPtr<Node> Node::load(const core::String_t &name) {
  auto media = core::Singleton<runtime::Media>::instance();
  auto buf = media->load(fmt::format("document::{}", name))->read();
  xmlNodePtr root = nullptr;
  xmlDocPtr document =
      xmlParseMemory((core::CString_t)buf->getData(), buf->getSize());
  if (!document) {
    return nullptr;
  }
  root = xmlDocGetRootElement(document);
  core::AutoPtr<Node> result = parseXML(root);
  xmlFreeDoc(document);
  return result;
}