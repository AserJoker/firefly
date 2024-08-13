#pragma once
#include "core/Properties.hpp"
#include "script/Script.hpp"
namespace firefly::script {
class Trait_Properties {
private:
  static FUNC_DEF(getKey);
  static FUNC_DEF(setKey);
  static FUNC_DEF(removeKey);
  static FUNC_DEF(store);

public:
  static void initialize(core::AutoPtr<Script> ctx);
  static core::AutoPtr<Value> create(core::AutoPtr<Script> ctx,
                                     core::AutoPtr<core::Properties> prop);
};
} // namespace firefly::script