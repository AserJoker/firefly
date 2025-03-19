#include "script/runtime/JSGeneratorConstructor.hpp"
#include "script/engine/JSEvalContext.hpp"
#include "script/engine/JSGenerator.hpp"
#include "script/engine/JSGeneratorFunction.hpp"
#include "script/engine/JSInterrupt.hpp"
#include "script/engine/JSInterruptType.hpp"
#include "script/engine/JSScope.hpp"
#include "script/engine/JSValue.hpp"
#include "script/engine/JSVirtualMachine.hpp"
#include <vector>
JS_CFUNCTION(JSGeneratorConstructor::constructor) {
  return ctx->createUndefined();
}
JS_CFUNCTION(JSGeneratorConstructor::return_) {
  auto result = ctx->createObject();
  auto generator = self->getData()->cast<JSGenerator>();
  auto func = generator->getFunction()->getData()->cast<JSGeneratorFunction>();
  auto &program = ctx->getRuntime()->getProgram(func->getProgramPath());
  JSInterrupt *interrupt = nullptr;
  if (generator->getInterrupt()) {
    interrupt = generator->getInterrupt()->getData()->cast<JSInterrupt>();
  }
  if (interrupt && interrupt->getEvalContext().pc == program.codes.size()) {

    ctx->setField(result, ctx->createString(L"done"), ctx->createBoolean(true));
    ctx->setField(result, ctx->createString(L"value"),
                  ctx->createValue(interrupt->getValue()));
  } else {
    JSValue *res = nullptr;
    auto current = ctx->getScope();
    auto vm = ctx->getRuntime()->getVirtualMachine();
    JSScope *scope = nullptr;
    JSEvalContext ectx;
    auto *pectx = &ectx;
    if (interrupt) {
      pectx = &interrupt->getEvalContext();
      scope = interrupt->getScope();
    } else {
      scope = ctx->getAllocator()->create<JSScope>(ctx->getRootScope());
      generator->setScope(scope);
      std::vector<JSValue *> args;
      for (auto &atom : generator->getArgs()) {
        args.push_back(ctx->createValue(atom));
      }
      if (func->getClass()) {
        ectx.clazz = ctx->createValue(func->getClass());
      }
      ectx.self = ctx->createValue(func->getSelf() ? func->getSelf()
                                                   : generator->getSelf());
      ectx.pc = program.codes.size();
      pectx = &ectx;
    }
    ctx->setScope(scope);
    if (args.empty()) {
      pectx->stack.push_back(ctx->createUndefined());
    } else {
      pectx->stack.push_back(args[0]);
    }
    pectx->pc = program.codes.size();
    res = vm->eval(ctx, program, *pectx);
    res = current->createValue(res->getAtom());
    ctx->setScope(current);
    CHECK(ctx, res);
    if (res->isTypeof<JSInterruptType>()) {
      if (generator->getInterrupt()) {
        ctx->recycle(generator->getInterrupt());
        self->getAtom()->removeChild(generator->getInterrupt());
      }
      interrupt = res->getData()->cast<JSInterrupt>();
      generator->setInterrupt(res->getAtom());
      self->getAtom()->addChild(res->getAtom());
      ctx->setField(result, ctx->createString(L"done"),
                    ctx->createBoolean(false));
      ctx->setField(result, ctx->createString(L"value"),
                    ctx->createValue(interrupt->getValue()));
    } else {
      ctx->getAllocator()->dispose(generator->getScope());
      ctx->setField(result, ctx->createString(L"done"),
                    ctx->createBoolean(true));
      ctx->setField(result, ctx->createString(L"value"), res);
      if (interrupt) {
        ctx->recycle(interrupt->getValue());
        interrupt->setValue(res->getAtom());
        generator->getInterrupt()->addChild(res->getAtom());
        interrupt->getEvalContext().pc = program.codes.size();
      } else {
        auto inter = ctx->createInterrupt({.pc = program.codes.size()}, res);
        generator->setInterrupt(inter->getAtom());
        self->getAtom()->addChild(inter->getAtom());
      }
    }
  }
  return result;
}
JS_CFUNCTION(JSGeneratorConstructor::throw_) {
  auto result = ctx->createObject();
  auto generator = self->getData()->cast<JSGenerator>();
  auto func = generator->getFunction()->getData()->cast<JSGeneratorFunction>();
  auto &program = ctx->getRuntime()->getProgram(func->getProgramPath());
  JSInterrupt *interrupt = nullptr;
  if (generator->getInterrupt()) {
    interrupt = generator->getInterrupt()->getData()->cast<JSInterrupt>();
  }
  if (interrupt && interrupt->getEvalContext().pc == program.codes.size()) {

    ctx->setField(result, ctx->createString(L"done"), ctx->createBoolean(true));
    ctx->setField(result, ctx->createString(L"value"),
                  ctx->createValue(interrupt->getValue()));
  } else {
    JSValue *res = nullptr;
    auto current = ctx->getScope();
    auto vm = ctx->getRuntime()->getVirtualMachine();
    JSScope *scope = nullptr;
    JSEvalContext ectx;
    auto *pectx = &ectx;
    if (interrupt) {
      pectx = &interrupt->getEvalContext();
      scope = interrupt->getScope();
    } else {
      scope = ctx->getAllocator()->create<JSScope>(ctx->getRootScope());
      generator->setScope(scope);
      std::vector<JSValue *> args;
      for (auto &atom : generator->getArgs()) {
        args.push_back(ctx->createValue(atom));
      }
      if (func->getClass()) {
        ectx.clazz = ctx->createValue(func->getClass());
      }
      ectx.self = ctx->createValue(func->getSelf() ? func->getSelf()
                                                   : generator->getSelf());
      ectx.pc = program.codes.size();
      pectx = &ectx;
    }
    ctx->setScope(scope);
    if (args.empty()) {
      pectx->stack.push_back(ctx->createException(ctx->createUndefined()));
    } else {
      pectx->stack.push_back(ctx->createException(args[0]));
    }
    pectx->pc = program.codes.size();
    res = vm->eval(ctx, program, *pectx);
    res = current->createValue(res->getAtom());
    ctx->setScope(current);
    CHECK(ctx, res);
    if (res->isTypeof<JSInterruptType>()) {
      if (generator->getInterrupt()) {
        ctx->recycle(generator->getInterrupt());
        self->getAtom()->removeChild(generator->getInterrupt());
      }
      interrupt = res->getData()->cast<JSInterrupt>();
      generator->setInterrupt(res->getAtom());
      self->getAtom()->addChild(res->getAtom());
      ctx->setField(result, ctx->createString(L"done"),
                    ctx->createBoolean(false));
      ctx->setField(result, ctx->createString(L"value"),
                    ctx->createValue(interrupt->getValue()));
    } else {
      ctx->getAllocator()->dispose(generator->getScope());
      ctx->setField(result, ctx->createString(L"done"),
                    ctx->createBoolean(true));
      ctx->setField(result, ctx->createString(L"value"), res);
      if (interrupt) {
        ctx->recycle(interrupt->getValue());
        interrupt->setValue(res->getAtom());
        generator->getInterrupt()->addChild(res->getAtom());
        interrupt->getEvalContext().pc = program.codes.size();
      } else {
        auto inter = ctx->createInterrupt({.pc = program.codes.size()}, res);
        generator->setInterrupt(inter->getAtom());
        self->getAtom()->addChild(inter->getAtom());
      }
    }
  }
  return result;
}
JS_CFUNCTION(JSGeneratorConstructor::next) {
  auto result = ctx->createObject();
  auto generator = self->getData()->cast<JSGenerator>();
  auto func = generator->getFunction()->getData()->cast<JSGeneratorFunction>();
  auto &program = ctx->getRuntime()->getProgram(func->getProgramPath());
  JSInterrupt *interrupt = nullptr;
  if (generator->getInterrupt()) {
    interrupt = generator->getInterrupt()->getData()->cast<JSInterrupt>();
  }
  JSValue *res = nullptr;
  auto current = ctx->getScope();
  auto vm = ctx->getRuntime()->getVirtualMachine();
  if (interrupt) {
    auto &ectx = interrupt->getEvalContext();
    if (ectx.pc == program.codes.size()) {
      ctx->setField(result, ctx->createString(L"done"),
                    ctx->createBoolean(true));
      ctx->setField(result, ctx->createString(L"value"),
                    ctx->createValue(interrupt->getValue()));
    } else {
      auto scope = interrupt->getScope();
      ctx->setScope(scope);
      if (args.empty()) {
        ectx.stack.push_back(ctx->createUndefined());
      } else {
        ectx.stack.push_back(ctx->createValue(args[0]));
      }
      res = vm->eval(ctx, program, ectx);
    }
  } else {
    auto scope = ctx->getAllocator()->create<JSScope>(ctx->getRootScope());
    generator->setScope(scope);
    ctx->setScope(scope);
    std::vector<JSValue *> args;
    for (auto &atom : generator->getArgs()) {
      args.push_back(ctx->createValue(atom));
    }
    auto self = ctx->createValue(func->getSelf() ? func->getSelf()
                                                 : generator->getSelf());
    auto clazz =
        func->getClass() ? ctx->createValue(func->getClass()) : nullptr;
    res = vm->eval(ctx, program,
                   {
                       .pc = func->getAddress(),
                       .self = self,
                       .clazz = clazz,
                   });
  }
  if (res) {
    res = current->createValue(res->getAtom());
    ctx->setScope(current);
    CHECK(ctx, res);
    if (res->isTypeof<JSInterruptType>()) {
      if (generator->getInterrupt()) {
        ctx->recycle(generator->getInterrupt());
        self->getAtom()->removeChild(generator->getInterrupt());
      }
      interrupt = res->getData()->cast<JSInterrupt>();
      generator->setInterrupt(res->getAtom());
      self->getAtom()->addChild(res->getAtom());
      ctx->setField(result, ctx->createString(L"done"),
                    ctx->createBoolean(false));
      ctx->setField(result, ctx->createString(L"value"),
                    ctx->createValue(interrupt->getValue()));
    } else {
      ctx->getAllocator()->dispose(generator->getScope());
      ctx->setField(result, ctx->createString(L"done"),
                    ctx->createBoolean(true));
      ctx->setField(result, ctx->createString(L"value"), res);
      if (interrupt) {
        ctx->recycle(interrupt->getValue());
        interrupt->setValue(res->getAtom());
        generator->getInterrupt()->addChild(res->getAtom());
        interrupt->getEvalContext().pc = program.codes.size();
      } else {
        auto inter = ctx->createInterrupt({.pc = program.codes.size()}, res);
        generator->setInterrupt(inter->getAtom());
        self->getAtom()->addChild(inter->getAtom());
      }
    }
  }
  return result;
}
JSValue *JSGeneratorConstructor::initialize(JSContext *ctx) {
  auto Generator = ctx->createNativeFunction(&constructor, L"Generator");
  auto prototype = ctx->getField(Generator, ctx->createString(L"prototype"));
  CHECK(ctx, prototype);
  auto err = ctx->setField(prototype, ctx->createString(L"next"),
                           ctx->createNativeFunction(&next, L"next"));
  CHECK(ctx, err);
  err = ctx->setField(prototype, ctx->createString(L"throw"),
                      ctx->createNativeFunction(&throw_, L"throw"));
  CHECK(ctx, err);
  err = ctx->setField(prototype, ctx->createString(L"return"),
                      ctx->createNativeFunction(&return_, L"return"));
  CHECK(ctx, err);
  return Generator;
}