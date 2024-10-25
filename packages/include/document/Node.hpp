#pragma once
#include "Attribute.hpp"
#include "core/Array.hpp"
#include "core/AutoPtr.hpp"
#include "core/Map.hpp"
#include "core/Object.hpp"
#include "core/Point.hpp"
#include "core/Rect.hpp"
#include "core/Value.hpp"
namespace firefly::document {
class Node : public core::Object {
private:
  static core::Map<core::String_t, Node *> _indexedNodes;

private:
  core::Array<core::AutoPtr<Node>> _children;
  Node *_parent;
  core::String_t _id;

  core::Map<core::String_t, Attribute> _attributes;

  bool _ready;

  core::String_t _currentAttributeGroup;
  core::Array<core::String_t> _attributeGroupStack;

protected:
  template <class T>
  void defineAttribute(const core::String_t &name, T &attribute) {
    _attributes[name] = &attribute;
  }

  void defineAttribute(const core::String_t &name, core::Rect<> &attribute) {
    defineAttribute(name + ".x", attribute.x);
    defineAttribute(name + ".y", attribute.y);
    defineAttribute(name + ".width", attribute.width);
    defineAttribute(name + ".height", attribute.height);
  }

  void defineAttribute(const core::String_t &name, core::Point<> &attribute) {
    defineAttribute(name + ".x", attribute.x);
    defineAttribute(name + ".y", attribute.y);
  }

  void defineAttribute(const core::String_t &name, core::Size<> &attribute) {
    defineAttribute(name + ".width", attribute.width);
    defineAttribute(name + ".height", attribute.height);
  }

  bool isAttribute(const core::String_t &key, const core::String_t &attribute) {
    return attribute == key || attribute.starts_with(key + ".");
  }

  template <class T> T *findParent() {
    auto parent = _parent;
    while (parent) {
      auto p = dynamic_cast<T *>(parent);
      if (p != nullptr) {
        return p;
      }
      parent = parent->_parent;
    }
    return nullptr;
  }

  virtual void onAttrChange(const core::String_t &name) {};

public:
  Node();
  ~Node() override;

  const core::Array<core::AutoPtr<Node>> &getChildren() const;
  core::Array<core::AutoPtr<Node>> &getChildren();

  core::AutoPtr<Node> getParent();
  const core::AutoPtr<Node> getParent() const;

  void setIdentity(const core::String_t &id);
  const core::String_t &getIdentity() const;

  void appendChild(core::AutoPtr<Node> child);
  void removeChild(core::AutoPtr<Node> child);

  const core::Value getAttribute(const core::String_t &name) const;
  void setAttribute(const core::String_t &name, const core::Value &value);
  inline void setAttribute(const core::String_t &name,
                           const core::Rect<> &rect) {
    beginAttributeGroup(name);
    setAttribute("x", rect.x);
    setAttribute("y", rect.y);
    setAttribute("width", rect.width);
    setAttribute("height", rect.height);
    endAttributeGroup();
  }
  inline void setAttribute(const core::String_t &name,
                           const core::Size<> &size) {
    beginAttributeGroup(name);
    setAttribute("width", size.width);
    setAttribute("height", size.height);
    endAttributeGroup();
  }
  inline void setAttribute(const core::String_t &name,
                           const core::Point<> &position) {
    beginAttributeGroup(name);
    setAttribute("x", position.x);
    setAttribute("y", position.y);
    endAttributeGroup();
  }
  void beginAttributeGroup(const core::String_t &name);
  void endAttributeGroup();

  void onMainLoop();

protected:
  virtual void onTick();
  virtual void onLoad();
  virtual void onUnload();

public:
  static core::AutoPtr<Node> select(const core::String_t &identity);
};
}; // namespace firefly::document