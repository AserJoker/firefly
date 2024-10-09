#pragma once
#include "script/Script.hpp"
#include "video/Animation.hpp"
namespace firefly::script {
class Trait_Animation {
private:
  static FUNC_DEF(setAction);
  static FUNC_DEF(removeAction);
  static FUNC_DEF(start);
  static FUNC_DEF(stop);
  static FUNC_DEF(resume);
  static FUNC_DEF(reset);
  static FUNC_DEF(setFPS);
  static FUNC_DEF(setGroup);

public:
  static core::AutoPtr<Value>
  create(core::AutoPtr<Script> ctx,
         const core::AutoPtr<video::Animation> &animation);
  static void initialize(core::AutoPtr<Script> ctx);
};
} // namespace firefly::script