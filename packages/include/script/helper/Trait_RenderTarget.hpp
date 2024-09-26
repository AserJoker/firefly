#pragma once
#include "core/AutoPtr.hpp"
#include "script/Script.hpp"
#include "script/Value.hpp"
#include "video/RenderTarget.hpp"
namespace firefly::script {
class Trait_RenderTarget {
private:
public:
  static void initialize(core::AutoPtr<Script> script);
  static core::AutoPtr<Value>
  create(core::AutoPtr<Script> ctx,
         const core::AutoPtr<video::RenderTarget> &renderTarget);
};
} // namespace firefly::script