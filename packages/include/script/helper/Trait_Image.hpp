#pragma once
#include "script/Script.hpp"
#include "video/Image.hpp"
namespace firefly::script {
class Trait_Image {
private:
  static FUNC_DEF(getSize);

public:
  static core::AutoPtr<Value> create(core::AutoPtr<Script> ctx,
                                     core::AutoPtr<video::Image> buffer);
  static void initialize(core::AutoPtr<Script> ctx);
};
}; // namespace firefly::script