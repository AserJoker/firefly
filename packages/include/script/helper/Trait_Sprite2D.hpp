#pragma once
#include "script/Script.hpp"
#include "video/Sprite2D.hpp"
namespace firefly::script {
class Trait_Sprite2D {
private:
  static FUNC_DEF(setRect);
  static FUNC_DEF(setSourceRect);
  static FUNC_DEF(setTexture);
  static FUNC_DEF(setRotation);

public:
  static void initialize(core::AutoPtr<Script> ctx);
  static core::AutoPtr<Value> create(core::AutoPtr<Script> ctx,
                                     core::AutoPtr<video::Sprite2D> sprite);
};
}; // namespace firefly::script