#include "script/lib/Module_Video.hpp"
#include "exception/ValidateException.hpp"
#include "script/Atom.hpp"
#include "script/Script.hpp"
#include "script/helper/Trait_Image.hpp"
#include "video/Image.hpp"
#include "video/ImageFormat.hpp"
using namespace firefly;
using namespace firefly::script;
using exception::ValidateException;
void Module_Video::open(core::AutoPtr<Script> ctx) {
  auto exports =
      ctx->createValue()->setObject(ctx)->setFunctionField(ctx, createImage);
  auto format =
      ctx->createValue()
          ->setObject(ctx)
          ->setField(ctx, "RGB", ctx->createValue()->setNumber(ctx, 0))
          ->setField(ctx, "RGBA", ctx->createValue()->setNumber(ctx, 1))
          ->setField(ctx, "GRAY", ctx->createValue()->setNumber(ctx, 2));
  exports->setField(ctx, "format", format);
  ctx->registerModule("video", exports);
}
FUNC_DEF(Module_Video::createImage) {
  VALIDATE_ARGS(createImage, 1);
  if (args[0]->getType(ctx) == Atom::TYPE::STRING) {
    auto name = args[0]->toString(ctx);
    return {Trait_Image::create(ctx, new video::Image(name))};
  } else {
    VALIDATE_ARGS(createImage, 3);
    auto width = args[0]->toNumber(ctx);
    auto height = args[1]->toNumber(ctx);
    video::IMAGE_FORMAT fmt = (video::IMAGE_FORMAT)args[2]->toNumber(ctx);
    return {Trait_Image::create(ctx, new video::Image(width, height, fmt))};
  }
}