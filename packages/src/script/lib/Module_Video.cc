#include "script/lib/Module_Video.hpp"
#include "core/AutoPtr.hpp"
#include "exception/ValidateException.hpp"
#include "script/Script.hpp"
#include "script/helper/Trait_Sprite2D.hpp"
#include "video/Sprite2D.hpp"
using namespace firefly;
using namespace firefly::script;
using exception::ValidateException;

FUNC_DEF(Module_Video::createSprite2D) {
  VALIDATE_ARGS(createSprite2D, 1);
  auto path = args[0]->toString(ctx);
  core::AutoPtr<video::Sprite2D> sprite = new video::Sprite2D(path);
  return {Trait_Sprite2D::create(ctx, sprite)};
}

void Module_Video::open(core::AutoPtr<Script> ctx) {
  auto exports = ctx->createValue()->setObject(ctx);

  auto format =
      ctx->createValue()
          ->setObject(ctx)
          ->setField(ctx, "RGB", ctx->createValue()->setNumber(ctx, 0))
          ->setField(ctx, "RGBA", ctx->createValue()->setNumber(ctx, 1))
          ->setField(ctx, "GRAY", ctx->createValue()->setNumber(ctx, 2));

  exports->setField(ctx, "format", format)->setFunction(ctx, createSprite2D);

  ctx->registerModule("video", exports);
}