#pragma once
#include "script/Script.hpp"
#include "video/Camera.hpp"
namespace firefly::script {
class Trait_Camera {
private:
  static FUNC_DEF(getPosition);
  static FUNC_DEF(setPosition);

public:
  static void initialize(core::AutoPtr<Script> ctx);
  static core::AutoPtr<Value> create(core::AutoPtr<Script> ctx,
                                     core::AutoPtr<video::Camera> camera);
};
} // namespace firefly::script