#include "script/helper/Trait_Image.hpp"
#include "exception/ValidateException.hpp"
#include "video/Image.hpp"
using namespace firefly;
using namespace firefly::script;
using exception::ValidateException;
core::AutoPtr<Value> Trait_Image::create(core::AutoPtr<Script> ctx,
                                         core::AutoPtr<video::Image> image) {
  auto global = ctx->getNativeGlobal();
  auto Image = global->getField(ctx, "Image");
  auto instance = ctx->createValue()->setObject(ctx)->setOpaque(image);
  for (auto &field : Image->getKeys(ctx)) {
    instance->setField(ctx, field, Image->getField(ctx, field));
  }
  return instance;
}
void Trait_Image::initialize(core::AutoPtr<Script> ctx) {
  auto global = ctx->getNativeGlobal();
  auto Image =
      ctx->createValue()->setObject(ctx)->setFunctionField(ctx, getSize);
  global->setField(ctx, "Image", Image);
}
FUNC_DEF(Trait_Image::getSize) {
  VALIDATE_ARGS(getSize, 1);
  auto self = args[0];
  auto image = self->getOpaque().cast<video::Image>();
  auto width = image->getWidth();
  auto height = image->getHeight();
  return {ctx->createValue()->setNumber(ctx, width),
          ctx->createValue()->setNumber(ctx, height)};
}