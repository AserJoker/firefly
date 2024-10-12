#pragma once
#include "core/Attribute.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <fmt/format.h>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>
namespace firefly::document {

class Node : public core::Object {
public:
  struct AttrBinding {
    std::string attr;
    Node *node;
  };

private:
  Node *_parent;
  std::list<core::AutoPtr<Node>> _children;
  std::unordered_map<std::string, core::AutoPtr<Node>> _indexedChildren;
  std::unordered_map<std::string, core::PtrAttribute> _attributes;
  std::unordered_map<std::string, std::vector<AttrBinding>> _bindings;
  std::vector<Node *> _bindingHosts;
  std::unordered_map<std::string, std::vector<std::string>> _attributeGroups;
  std::string _id;

  std::list<std::string> _attrGroups;
  std::string _attrGroup;
  uint32_t _changed;

protected:
  virtual void onAttrChange(const std::string &name);
  template <class T>
  inline void defineAttribute(const std::string &name, T *field) {
    _attributes[name] = core::PtrAttribute(field);
    auto pos = name.find_first_of('.');
    if (pos != std::string::npos) {
      auto groupName = name.substr(0, pos);
      auto &group = this->_attributeGroups[groupName];
      group.push_back(name);
    }
  }

public:
  Node();
  ~Node() override;
  void setId(const std::string &id);
  const std::string &getId() const;
  core::AutoPtr<Node> getChild(const std::string &id);
  const core::AutoPtr<Node> getChild(const std::string &id) const;
  void appendChild(core::AutoPtr<Node> node);
  void removeChild(core::AutoPtr<Node> node);
  core::AutoPtr<Node> getParent();
  std::list<core::AutoPtr<Node>> &getChildren();
  const core::Attribute getAttribute(const std::string &name) const;
  void beginAttrGroup(const std::string &name);
  bool setAttribute(const std::string &name, const core::PtrAttribute &value);
  bool setAttribute(const std::string &name, const core::Attribute &value);
  void endAttrGroup();
  void bindAttribute(const std::string name, core::AutoPtr<Node> host,
                     const std::string &source);
  const std::unordered_map<std::string, std::string> getAttributes() const;
  const std::unordered_map<std::string, std::vector<std::string>> &
  getAttributeGroups() const;
  virtual void onTick();
};
}; // namespace firefly::document