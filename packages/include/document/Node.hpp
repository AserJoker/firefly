#pragma once
#include "core/AnyPtr.hpp"
#include "core/Array.hpp"
#include "core/AutoPtr.hpp"
#include "core/Map.hpp"
#include "core/Object.hpp"
#include "core/Value.hpp"
namespace firefly::document {
class Node : public core::Object {
private:
  static core::Map<core::String_t, Node *> _indexedNodes;

private:
  core::Array<core::AutoPtr<Node>> _children;
  Node *_parent;
  core::String_t _id;

  core::Map<core::String_t, core::AnyPtr> _attributes;

  bool _ready;

protected:
  template <class T>
  void defineAttribute(const core::String_t &name, T &attribute) {
    _attributes[name] = &attribute;
  }

  template <class T> core::AutoPtr<T> findParent() {
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

public:
  virtual void onTick();
  virtual void onLoad();
  virtual void onUnload();

public:
  static core::AutoPtr<Node> select(const core::String_t &identity);
};
}; // namespace firefly::document