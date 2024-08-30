#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <vector>
namespace firefly::video {
class Renderer;
class Object3D : public core::Object {
private:
  std::vector<core::AutoPtr<Object3D>> _children;
  Object3D *_parent;

public:
  Object3D();
  void add(const core::AutoPtr<Object3D> &child);
  void remove(const core::AutoPtr<Object3D> &child);
  const std::vector<core::AutoPtr<Object3D>> &getChildren() const;
  const core::AutoPtr<Object3D> getParent() const;
  virtual void draw(core::AutoPtr<Renderer> renderer);
};
} // namespace firefly::video