#pragma once
#include "core/AutoPtr.hpp"
#include "document/RenderTarget.hpp"
#include "script/Script.hpp"
#include "script/Value.hpp"

namespace firefly::script {
class Trait_RenderTarget {
private:
  static FUNC_DEF(resize);

public:
  static void initialize(core::AutoPtr<Script> script);
  static core::AutoPtr<Value>
  create(core::AutoPtr<Script> ctx,
         const core::AutoPtr<document::RenderTarget> &renderTarget);
};
} // namespace firefly::script