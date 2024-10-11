#pragma once
#include "document/Sprite2D.hpp"
#include "script/Script.hpp"

namespace firefly::script {
class Trait_Sprite2D {
private:
  static FUNC_DEF(setTexture);
  static FUNC_DEF(getTextureSize);
  static FUNC_DEF(setRect);
  static FUNC_DEF(getRect);
  static FUNC_DEF(setSourceRect);
  static FUNC_DEF(getSourceRect);
  static FUNC_DEF(setRotation);
  static FUNC_DEF(getRotation);
  static FUNC_DEF(setVisible);
  static FUNC_DEF(isVisible);
  static FUNC_DEF(setZIndex);
  static FUNC_DEF(getZIndex);
  static FUNC_DEF(setBlend);
  static FUNC_DEF(getBlend);

public:
  static void initialize(core::AutoPtr<Script> ctx);
  static core::AutoPtr<Value> create(core::AutoPtr<Script> ctx,
                                     core::AutoPtr<document::Sprite2D> sprite);
};
}; // namespace firefly::script