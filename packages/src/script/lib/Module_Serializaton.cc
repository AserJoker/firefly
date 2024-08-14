#include "core/AutoPtr.hpp"
#include "exception/ValidateException.hpp"
#include "script/Script.hpp"
#include "script/Value.hpp"
#include "script/lib/Module_Serialization.hpp"
#include <fmt/core.h>
using namespace firefly;
using namespace firefly::script;
using exception::ValidateException;
void Module_Serialization::open(core::AutoPtr<Script> ctx) {
  auto exports = ctx->createValue()->setObject(ctx);
  auto json =
      ctx->createValue()
          ->setObject(ctx)
          ->setField(ctx, "stringify",
                     ctx->createValue()->setFunction(ctx, JSON_stringify))
          ->setField(ctx, "parse",
                     ctx->createValue()->setFunction(ctx, JSON_parse));
  auto yaml =
      ctx->createValue()
          ->setObject(ctx)
          ->setField(ctx, "stringify",
                     ctx->createValue()->setFunction(ctx, YAML_stringify))
          ->setField(ctx, "parse",
                     ctx->createValue()->setFunction(ctx, YAML_parse));
  exports->setField(ctx, "JSON", json);
  exports->setField(ctx, "YAML", yaml);
  ctx->registerModule("serialization", exports);
}

FUNC_DEF(Module_Serialization::JSON_stringify) {
  VALIDATE_ARGS(load, 1);
  return {createString(ctx, args[0]->toJSON(ctx))};
}
FUNC_DEF(Module_Serialization::JSON_parse) {
  VALIDATE_ARGS(load, 1);
  return {Value::parseJSON(ctx, args[0]->toString(ctx))};
}

FUNC_DEF(Module_Serialization::YAML_stringify) {
  VALIDATE_ARGS(load, 1);
  return {createString(ctx, args[0]->toYAML(ctx))};
}
FUNC_DEF(Module_Serialization::YAML_parse) {
  VALIDATE_ARGS(load, 1);
  return {Value::parseYAML(ctx, args[0]->toString(ctx))};
}