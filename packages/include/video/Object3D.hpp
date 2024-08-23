#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "video/Renderer.hpp"
#include <vector>
namespace firefly::video {
class Object3D : public core::Object {
private:
  std::vector<core::AutoPtr<Object3D>> _children;
  Object3D *_parent;
  bool _visible;

public:
  Object3D();
  void add(const core::AutoPtr<Object3D> &child);
  void remove(const core::AutoPtr<Object3D> &child);
  core::AutoPtr<Object3D> getParent();
  const std::vector<core::AutoPtr<Object3D>> &getChildren() const;
  std::vector<core::AutoPtr<Object3D>> &getChildren();
  void setVisible(bool value);
  const bool &isVisible() const;
  virtual void draw(core::AutoPtr<Renderer> renderer) = 0;
};
}; // namespace firefly::video