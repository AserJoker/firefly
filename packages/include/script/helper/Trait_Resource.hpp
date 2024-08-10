#pragma once
#include "core/AutoPtr.hpp"
#include "runtime/Resource.hpp"
#include "script/Script.hpp"
#include "script/Value.hpp"
namespace firefly::script {
class Trait_Resource {
private:
  static FUNC_DEF(read);

public:
  static core::AutoPtr<Value> create(core::AutoPtr<Script> ctx,
                                     core::AutoPtr<runtime::Resource> resource);
};
}; // namespace firefly::script