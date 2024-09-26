#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <list>
namespace firefly::video {
class Node : public core::Object {
private:
  Node *_parent;
  std::list<core::AutoPtr<Node>> _children;

public:
  Node();
  void appendChild(core::AutoPtr<Node> node);
  void removeChild(core::AutoPtr<Node> node);
  core::AutoPtr<Node> getParent();
  std::list<core::AutoPtr<Node>> &getChildren();
  virtual void onTick();
};
}; // namespace firefly::video