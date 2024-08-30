#pragma once
#include "Object3D.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
namespace firefly::video {
class Scene : public core::Object {
private:
  core::AutoPtr<Object3D> _root;

public:
  Scene();
  const core::AutoPtr<Object3D> &getRoot() const;
  core::AutoPtr<Object3D> &getRoot();
};
} // namespace firefly::video