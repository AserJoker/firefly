#include "script/engine/JSGeneratorFunctionType.hpp"
#include "script/engine/JSGenerator.hpp"
#include "script/util/JSAllocator.hpp"
#include <vector>

JSGeneratorFunctionType::JSGeneratorFunctionType(JSAllocator *allocator)
    : JSFunctionType(allocator) {}
    
JSValue *JSGeneratorFunctionType::call(JSContext *ctx, JSValue *func,
                                       JSValue *self,
                                       std::vector<JSValue *> args) const {
  auto generator = ctx->construct(ctx->getGeneratorConstructor());
  std::vector<JSAtom *> argv;
  for (auto &arg : args) {
    generator->getAtom()->addChild(arg->getAtom());
    argv.push_back(arg->getAtom());
  }
  generator->getAtom()->addChild(func->getAtom());
  generator->getAtom()->addChild(self->getAtom());
  auto gen = generator->getData()->cast<JSGenerator>();
  gen->setFunctionCall(func->getAtom(), self->getAtom(), argv);
  return generator;
};