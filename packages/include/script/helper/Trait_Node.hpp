#pragma once
#include "core/AutoPtr.hpp"
#include "script/Script.hpp"
#include "script/Value.hpp"
#include "video/Node.hpp"
namespace firefly::script {
class Trait_Node {
private:
  static FUNC_DEF(appendChild);
  static FUNC_DEF(removeChild);
  static FUNC_DEF(getParent);

public:
  static void initialize(core::AutoPtr<Script> script);
  static core::AutoPtr<Value> create(core::AutoPtr<Script> ctx,
                                     const core::AutoPtr<video::Node> &node);
};
} // namespace firefly::script