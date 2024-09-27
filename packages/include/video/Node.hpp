#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <list>
#include <unordered_map>
namespace firefly::video {
class Node : public core::Object {
private:
  Node *_parent;
  std::list<core::AutoPtr<Node>> _children;
  std::unordered_map<std::string, core::AutoPtr<Node>> _indexedChildren;
  std::string _id;

public:
  Node();
  void setId(const std::string &id);
  const std::string &getId() const;
  core::AutoPtr<Node> getChild(const std::string &id);
  const core::AutoPtr<Node> getChild(const std::string &id) const;
  void appendChild(core::AutoPtr<Node> node);
  void removeChild(core::AutoPtr<Node> node);
  core::AutoPtr<Node> getParent();
  std::list<core::AutoPtr<Node>> &getChildren();
  virtual void onTick();
};
}; // namespace firefly::video