#pragma once
#include "core/AutoPtr.hpp"
#include "core/Map.hpp"
#include "core/Object.hpp"
#include "core/Value.hpp"
#include <fmt/format.h>
#include <list>
#include <string>

namespace firefly::document {

class Node : public core::Object {
private:
  static core::Map<std::string, Node *> _indexedNodes;

public:
  static core::AutoPtr<Node> query(const std::string &id);

public:
  struct AttrBinding {
    std::string attr;
    Node *node;
  };

private:
  Node *_parent;
  std::list<core::AutoPtr<Node>> _children;
  core::Map<std::string, core::ValuePtr> _attributes;
  core::Map<std::string, core::Array<AttrBinding>> _bindings;
  core::Array<Node *> _bindingHosts;
  core::Map<std::string, core::Array<std::string>> _attributeGroups;
  std::string _id;

  std::list<std::string> _attrGroups;
  std::string _attrGroup;

protected:
  virtual void onAttrChange(const std::string &name);
  template <class T>
  inline void defineAttribute(const std::string &name, T *field) {
    _attributes[name] = core::ValuePtr(field);
    auto pos = name.find_first_of('.');
    if (pos != std::string::npos) {
      auto groupName = name.substr(0, pos);
      auto &group = this->_attributeGroups[groupName];
      group.pushBack(name);
    }
  }

public:
  Node();
  ~Node() override;
  void setId(const std::string &id);
  const std::string &getId() const;
  void appendChild(core::AutoPtr<Node> node);
  void removeChild(core::AutoPtr<Node> node);
  core::AutoPtr<Node> getParent();
  std::list<core::AutoPtr<Node>> &getChildren();
  const core::Value getAttribute(const std::string &name) const;
  void beginAttrGroup(const std::string &name);
  bool setAttribute(const std::string &name, const core::ValuePtr &value);
  bool setAttribute(const std::string &name, const core::Value &value);
  void endAttrGroup();
  void bindAttribute(const std::string name, core::AutoPtr<Node> host,
                     const std::string &source);
  const core::Map<std::string, std::string> getAttributes() const;
  const core::Map<std::string, core::Array<std::string>> &
  getAttributeGroups() const;
  virtual void onTick();
};
}; // namespace firefly::document