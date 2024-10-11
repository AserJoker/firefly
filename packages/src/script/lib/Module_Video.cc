#include "script/lib/Module_Video.hpp"
#include "core/AutoPtr.hpp"
#include "exception/ValidateException.hpp"
#include "script/Script.hpp"

using namespace firefly;
using namespace firefly::script;
using exception::ValidateException;

void Module_Video::open(core::AutoPtr<Script> ctx) {
  auto exports = ctx->createValue()->setObject(ctx);

  ctx->registerModule("video", exports);
}