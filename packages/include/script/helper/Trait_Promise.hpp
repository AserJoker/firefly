#pragma once
#include "core/AutoPtr.hpp"
#include "core/Promise.hpp"
#include "script/Script.hpp"
#include "script/Value.hpp"
namespace firefly::script {
class Trait_Promise {
private:
  static FUNC_DEF(wait);
  static FUNC_DEF(next);
  static FUNC_DEF(resolve);
  static FUNC_DEF(reject);

public:
  static core::AutoPtr<Value> create(core::AutoPtr<Script> ctx,
                                     core::Promisify<core::AutoPtr<Value>> pro);
  static void initialize(core::AutoPtr<Script> ctx);
};
} // namespace firefly::script