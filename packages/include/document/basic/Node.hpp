#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <list>
namespace firefly::document {
class Node : public core::Object {
private:
  std::list<core::AutoPtr<Node>> _children;
  Node *_parent;

public:
  Node(Node *parent = nullptr);
  Node(const core::AutoPtr<Node> &parent = nullptr);
  void appendChild(const core::AutoPtr<Node> &child);
  void removeChild(const core::AutoPtr<Node> &child);
  const std::list<core::AutoPtr<Node>> &getChildren() const;
  std::list<core::AutoPtr<Node>> &getChildren();
  const core::AutoPtr<Node> getParent() const;
  core::AutoPtr<Node> getParent();
  void dispatch();
};
} // namespace firefly::document