#pragma once
#include "script/Script.hpp"
#include "document/Animation.hpp"
namespace firefly::script {
class Trait_Animation {
private:
  static FUNC_DEF(setAction);
  static FUNC_DEF(removeAction);
  static FUNC_DEF(start);
  static FUNC_DEF(stop);
  static FUNC_DEF(resume);
  static FUNC_DEF(reset);
  static FUNC_DEF(setSpeed);
  static FUNC_DEF(setGroup);

public:
  static core::AutoPtr<Value>
  create(core::AutoPtr<Script> ctx,
         const core::AutoPtr<document::Animation> &animation);
  static void initialize(core::AutoPtr<Script> ctx);
};
} // namespace firefly::script