#include "script/lib/Module_Video.hpp"
#include "exception/ValidateException.hpp"
#include "script/Script.hpp"
using namespace firefly;
using namespace firefly::script;
using exception::ValidateException;
void Module_Video::open(core::AutoPtr<Script> ctx) {
  auto scope = ctx->pushScope();
  auto exports = ctx->createValue()->setObject(ctx);
  auto format =
      ctx->createValue()
          ->setObject(ctx)
          ->setField(ctx, "RGB", ctx->createValue()->setNumber(ctx, 0))
          ->setField(ctx, "RGBA", ctx->createValue()->setNumber(ctx, 1))
          ->setField(ctx, "GRAY", ctx->createValue()->setNumber(ctx, 2));
  exports->setField(ctx, "format", format);
  ctx->registerModule("video", exports);
  ctx->popScope(scope);
}