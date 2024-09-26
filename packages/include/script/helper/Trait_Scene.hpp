#pragma once
#include "core/AutoPtr.hpp"
#include "script/Script.hpp"
#include "script/Value.hpp"
#include "video/Scene.hpp"
namespace firefly::script {
class Trait_Scene {
private:
  static FUNC_DEF(setCamera);

public:
  static void initialize(core::AutoPtr<Script> script);
  static core::AutoPtr<Value> create(core::AutoPtr<Script> ctx,
                                     const core::AutoPtr<video::Scene> &scene);
};
} // namespace firefly::script