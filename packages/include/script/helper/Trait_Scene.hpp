#pragma once
#include "core/AutoPtr.hpp"
#include "document/Scene.hpp"
#include "script/Script.hpp"
#include "script/Value.hpp"

namespace firefly::script {
class Trait_Scene {
private:
  static FUNC_DEF(setCamera);

public:
  static void initialize(core::AutoPtr<Script> script);
  static core::AutoPtr<Value>
  create(core::AutoPtr<Script> ctx,
         const core::AutoPtr<document::Scene> &scene);
};
} // namespace firefly::script