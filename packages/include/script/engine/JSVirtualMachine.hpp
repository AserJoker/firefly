#pragma once
#include "../compiler/JSProgram.hpp"
#include "../util/JSAllocator.hpp"
#include "JSContext.hpp"
#include "JSEvalContext.hpp"
#include "JSException.hpp"
#include "JSExceptionType.hpp"
#include "JSObject.hpp"
#include "JSValue.hpp"
#include "script/compiler/JSOperator.hpp"
#include "script/engine/JSCallable.hpp"
#include "script/engine/JSCallableType.hpp"
#include "script/engine/JSEvalContext.hpp"
#include "script/engine/JSException.hpp"
#include "script/engine/JSExceptionType.hpp"
#include "script/engine/JSInterruptType.hpp"
#include "script/engine/JSNullType.hpp"
#include "script/engine/JSValue.hpp"
#include "script/util/JSSingleton.hpp"
#include <cstdint>
#include <vector>

class JSVirtualMachine {
private:
  JSAllocator *_allocator;

private:
  uint32_t getUint32(const JSProgram &program, size_t &address) {
    auto val = *(uint32_t *)(program.codes.data() + address);
    address += 2;
    return val;
  }
  size_t getAddress(const JSProgram &program, size_t &address) {
    auto val = *(size_t *)(program.codes.data() + address);
    address += 4;
    return val;
  }
  double getNumber(const JSProgram &program, size_t &address) {
    auto val = *(double *)(program.codes.data() + address);
    address += 4;
    return val;
  }
  const std::wstring &getString(const JSProgram &program, size_t &address) {
    auto idx = getUint32(program, address);
    return program.constants[idx];
  }

  bool checkException(JSContext *ctx, JSValue *value, JSEvalContext &ectx,
                      const JSProgram &program) {
    if (ctx->isException(value)) {
      ectx.pc = program.codes.size();
      ectx.stack.push_back(value);
      return true;
    }
    return false;
  }

private:
  JSValue *call(JSContext *ctx, JSValue *func, JSValue *self,
                std::vector<JSValue *> args, const JSStackFrame &frame) {
    ctx->pushCallStack(frame.filename, frame.position.funcname,
                       frame.position.column, frame.position.line);
    auto res = ctx->call(func, self, args);
    ctx->popCallStack();
    return res;
  }
  JSValue *construct(JSContext *ctx, JSValue *constructor,
                     std::vector<JSValue *> args, const JSStackFrame &frame) {
    ctx->pushCallStack(frame.filename, frame.position.funcname,
                       frame.position.column, frame.position.line);
    auto res = ctx->construct(constructor, args);
    ctx->popCallStack();
    return res;
  }

private:
  void runBegin(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    ctx->pushScope();
    ectx.frames.push_back(ectx.stack.size());
  }
  void runEnd(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto top = *ectx.frames.rbegin();
    ectx.frames.pop_back();
    while (ectx.stack.size() != top) {
      ectx.stack.pop_back();
    }
    ctx->popScope();
  }
  void runPush(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto val = getNumber(program, ectx.pc);
    ectx.stack.push_back(ctx->createNumber(val));
  }

  void runPop(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    ectx.stack.pop_back();
  }
  void runPushValue(JSContext *ctx, const JSProgram &program,
                    JSEvalContext &ectx) {
    auto idx = getUint32(program, ectx.pc);
    ectx.stack.push_back(*(ectx.stack.rbegin() + idx));
  }
  void runNil(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    ectx.stack.push_back(ctx->createNull());
  }

  void runUndefined(JSContext *ctx, const JSProgram &program,
                    JSEvalContext &ectx) {
    ectx.stack.push_back(ctx->createUndefined());
  }
  void runTrue(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    ectx.stack.push_back(ctx->createBoolean(true));
  }
  void runFalse(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    ectx.stack.push_back(ctx->createBoolean(false));
  }
  void runRegex(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
  }
  void runBigint(JSContext *ctx, const JSProgram &program,
                 JSEvalContext &ectx) {
    auto init = getString(program, ectx.pc);
    ectx.stack.push_back(ctx->createBigInt(init));
  }
  void runLoad(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto name = getString(program, ectx.pc);
    if (name == L"NaN") {
      ectx.stack.push_back(ctx->createNaN());
      return;
    } else if (name == L"Infinity") {
      ectx.stack.push_back(ctx->createInfinity());
      return;
    }
    auto val = ctx->queryValue(name);
    if (checkException(ctx, val, ectx, program)) {
      return;
    }
    ectx.stack.push_back(val);
  }
  void runStore(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto name = getString(program, ectx.pc);
    auto value = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto variable = ctx->queryValue(name);
    if (checkException(ctx, variable, ectx, program)) {
      return;
    }
    auto res = ctx->assigmentValue(variable, value);
    if (checkException(ctx, res, ectx, program)) {
      return;
    }
    ectx.stack.push_back(res);
  }
  void runRef(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto identifier = getString(program, ectx.pc);
    auto func = *ectx.stack.rbegin();
    auto callable = dynamic_cast<JSCallable *>(func->getData());
    auto val = ctx->queryValue(identifier);
    if (checkException(ctx, val, ectx, program)) {
      return;
    }
    auto old = callable->getClosure(identifier);
    if (old) {
      ctx->recycle(callable->getClosure(identifier));
    }
    callable->setClosure(identifier, val->getAtom());
    func->getAtom()->addChild(val->getAtom());
  }
  void runStr(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto str = getString(program, ectx.pc);
    ectx.stack.push_back(ctx->createString(str));
  }
  void runVar(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto name = getString(program, ectx.pc);
    auto val = ctx->createUndefined();
    ctx->getScope()->storeValue(name, val);
  }
  void runConst(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto name = getString(program, ectx.pc);
    auto val = ctx->createUninitialized();
    val->setConst(true);
    ctx->getScope()->storeValue(name, val);
  }
  void runLet(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto name = getString(program, ectx.pc);
    auto val = ctx->createUninitialized();
    ctx->getScope()->storeValue(name, val);
  }
  void runObject(JSContext *ctx, const JSProgram &program,
                 JSEvalContext &ectx) {
    ectx.stack.push_back(ctx->createObject());
  }
  void runArray(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    ectx.stack.push_back(ctx->createArray());
  }
  void runFunction(JSContext *ctx, const JSProgram &program,
                   JSEvalContext &ectx) {
    auto entry = getAddress(program, ectx.pc);
    auto func = ctx->createFunction(L"", program.filename, entry);
    ectx.stack.push_back(func);
  }
  void runArrow(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runAsyncArrow(JSContext *ctx, const JSProgram &program,
                     JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runAsyncfunction(JSContext *ctx, const JSProgram &program,
                        JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runGenerator(JSContext *ctx, const JSProgram &program,
                    JSEvalContext &ectx) {
    auto entry = getAddress(program, ectx.pc);
    auto func = ctx->createGeneratorFunction(L"", program.filename, entry);
    ectx.stack.push_back(func);
  }
  void runAsyncgenerator(JSContext *ctx, const JSProgram &program,
                         JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runEnable(JSContext *ctx, const JSProgram &program,
                 JSEvalContext &ectx) {}
  void runDisable(JSContext *ctx, const JSProgram &program,
                  JSEvalContext &ectx) {}
  void runGetField(JSContext *ctx, const JSProgram &program,
                   JSEvalContext &ectx) {
    auto obj = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto field = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    obj = ctx->pack(obj);
    auto result = ctx->getField(obj, field);
    if (result->getType() == JSSingleton::query<JSExceptionType>()) {
      ectx.stack.push_back(result);
      ectx.pc = program.codes.size();
      return;
    }
    ectx.stack.push_back(result);
  }
  void runSetField(JSContext *ctx, const JSProgram &program,
                   JSEvalContext &ectx) {
    auto obj = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto field = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto val = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    obj = ctx->pack(obj);
    auto result = ctx->setField(obj, field, val);
    if (checkException(ctx, result, ectx, program)) {
      ectx.stack.push_back(result);
      ectx.pc = program.codes.size();
      return;
    }
    ectx.stack.push_back(result);
  }

  void runSetMethod(JSContext *ctx, const JSProgram &program,
                    JSEvalContext &ectx) {
    auto obj = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto field = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto method = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    obj = ctx->pack(obj);
    auto result = ctx->setField(obj, field, method);
    if (result->getType() == JSSingleton::query<JSExceptionType>()) {
      ectx.stack.push_back(result);
      ectx.pc = program.codes.size();
      return;
    }
    ectx.stack.push_back(result);
  }

  void runSetAccessorGetter(JSContext *ctx, const JSProgram &program,
                            JSEvalContext &ectx) {
    auto obj = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto name = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto getter = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    obj = ctx->pack(obj);
    auto object = obj->getData()->cast<JSObject>();
    auto &fields = object->getFields();
    JSField *field = nullptr;
    for (auto &[keyAtom, f] : fields) {
      auto key = ctx->createValue(keyAtom);
      auto result = ctx->isEqual(key, name);
      if (checkException(ctx, result, ectx, program)) {
        return;
      }
      if (ctx->checkedBoolean(result)) {
        field = &f;
        break;
      }
    }
    if (field == nullptr) {
      auto pname = ctx->createValue(name);
      fields[pname->getAtom()] = {
          .configurable = true,
          .enumable = true,
          .getter = getter->getAtom(),
          .setter = nullptr,
      };
      obj->getAtom()->addChild(getter->getAtom());
    } else {
      if (field->value) {
        obj->getAtom()->removeChild(field->value);
        field->value = nullptr;
      }
      if (field->getter) {
        obj->getAtom()->removeChild(field->getter);
        field->getter = nullptr;
      }
      field->getter = getter->getAtom();
      obj->getAtom()->addChild(getter->getAtom());
    }
    ectx.stack.push_back(getter);
  }
  void runSetAccessorSetter(JSContext *ctx, const JSProgram &program,
                            JSEvalContext &ectx) {
    auto obj = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto name = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto setter = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    obj = ctx->pack(obj);
    auto object = obj->getData()->cast<JSObject>();
    auto &fields = object->getFields();
    JSField *field = nullptr;
    for (auto &[keyAtom, f] : fields) {
      auto key = ctx->createValue(keyAtom);
      auto result = ctx->isEqual(key, name);
      if (checkException(ctx, result, ectx, program)) {
        return;
      }
      if (ctx->checkedBoolean(result)) {
        field = &f;
        break;
      }
    }
    if (field == nullptr) {
      auto pname = ctx->createValue(name);
      fields[pname->getAtom()] = {
          .configurable = true,
          .enumable = true,
          .getter = nullptr,
          .setter = setter->getAtom(),
      };
      obj->getAtom()->addChild(setter->getAtom());
    } else {
      if (field->value) {
        obj->getAtom()->removeChild(field->value);
        field->value = nullptr;
      }
      if (field->setter) {
        obj->getAtom()->removeChild(field->setter);
        field->setter = nullptr;
      }
      field->setter = setter->getAtom();
      obj->getAtom()->addChild(setter->getAtom());
    }
    ectx.stack.push_back(setter);
  }
  void runSetPropField(JSContext *ctx, const JSProgram &program,
                       JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runSetPropMethod(JSContext *ctx, const JSProgram &program,
                        JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runSetPropAccessorGetter(JSContext *ctx, const JSProgram &program,
                                JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runSetPropAccessorSetter(JSContext *ctx, const JSProgram &program,
                                JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runCall(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto frame = program.stacks.at(ectx.pc);
    auto size = getUint32(program, ectx.pc);
    std::vector<JSValue *> arguments;
    for (size_t index = 0; index < size; index++) {
      auto val = *ectx.stack.rbegin();
      ectx.stack.pop_back();
      arguments.push_back(val);
    }
    auto func = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    frame.position.funcname = func->getData()->cast<JSCallable>()->getName();
    auto result = call(ctx, func, ctx->createUndefined(), arguments, frame);
    if (checkException(ctx, result, ectx, program)) {
      return;
    }
    ectx.stack.push_back(result);
  }
  void runMemberCall(JSContext *ctx, const JSProgram &program,
                     JSEvalContext &ectx) {
    auto frame = program.stacks.at(ectx.pc);
    auto size = getUint32(program, ectx.pc);
    std::vector<JSValue *> arguments;
    for (size_t index = 0; index < size; index++) {
      auto val = *ectx.stack.rbegin();
      ectx.stack.pop_back();
      arguments.push_back(val);
    }
    auto field = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto obj = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    obj = ctx->pack(obj);
    auto func = ctx->getField(obj, field);
    if (checkException(ctx, func, ectx, program)) {
      return;
    }
    frame.position.funcname = func->getData()->cast<JSCallable>()->getName();
    auto result = call(ctx, func, obj, arguments, frame);
    if (checkException(ctx, result, ectx, program)) {
      return;
    }
    ectx.stack.push_back(result);
  }
  void runPrivateMemberCall(JSContext *ctx, const JSProgram &program,
                            JSEvalContext &ectx) {

    // not implement
    ectx.pc = program.codes.size();
  }

  void runVoid(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    ectx.stack.pop_back();
    ectx.stack.push_back(ctx->createUndefined());
  }

  void runTypeof(JSContext *ctx, const JSProgram &program,
                 JSEvalContext &ectx) {
    auto value = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    ectx.stack.push_back(ctx->createString(value->getType()->getTypeName()));
  }

  void runNew(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto frame = program.stacks.at(ectx.pc);
    auto size = getUint32(program, ectx.pc);
    std::vector<JSValue *> arguments;
    for (size_t index = 0; index < size; index++) {
      auto val = *ectx.stack.rbegin();
      ectx.stack.pop_back();
      arguments.push_back(val);
    }
    auto constructor = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    frame.position.funcname =
        constructor->getData()->cast<JSCallable>()->getName();
    auto result = construct(ctx, constructor, arguments, frame);
    if (checkException(ctx, result, ectx, program)) {
      return;
    }
    ectx.stack.push_back(result);
  }

  void runDelete(JSContext *ctx, const JSProgram &program,
                 JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runRet(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    ectx.pc = program.codes.size();
  }
  void runYield(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto value = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto interrupt = ctx->createInterrupt(ectx, value);
    ectx.stack.push_back(interrupt);
    ectx.pc = program.codes.size();
  }
  void runAwait(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runYieldDelegate(JSContext *ctx, const JSProgram &program,
                        JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runJmp(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto address = getAddress(program, ectx.pc);
    ectx.pc = address;
  }
  void runJtrue(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto address = getAddress(program, ectx.pc);
    auto value = *ectx.stack.rbegin();
    auto boolean = ctx->toBoolean(value);
    if (checkException(ctx, boolean, ectx, program)) {
      return;
    }
    if (ctx->checkedBoolean(boolean)) {
      ectx.pc = address;
    }
  }
  void runJfalse(JSContext *ctx, const JSProgram &program,
                 JSEvalContext &ectx) {
    auto address = getAddress(program, ectx.pc);
    auto value = *ectx.stack.rbegin();
    auto boolean = ctx->toBoolean(value);
    if (checkException(ctx, boolean, ectx, program)) {
      return;
    }
    if (!ctx->checkedBoolean(boolean)) {
      ectx.pc = address;
    }
  }
  void runJnull(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto address = getAddress(program, ectx.pc);
    auto value = *ectx.stack.rbegin();
    if (value->isTypeof<JSNullType>()) {
      ectx.pc = address;
    }
  }
  void runJnotNull(JSContext *ctx, const JSProgram &program,
                   JSEvalContext &ectx) {
    auto address = getAddress(program, ectx.pc);
    auto value = *ectx.stack.rbegin();
    if (!value->isTypeof<JSNullType>()) {
      ectx.pc = address;
    }
  }
  void runUnaryPlus(JSContext *ctx, const JSProgram &program,
                    JSEvalContext &ectx) {
    auto value = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    ectx.stack.push_back(ctx->unaryPlus(value));
  }

  void runUnaryNegative(JSContext *ctx, const JSProgram &program,
                        JSEvalContext &ectx) {
    auto value = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    ectx.stack.push_back(ctx->unaryNegation(value));
  }

  void runAdd(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto right = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto left = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto val = ctx->add(left, right);
    if (checkException(ctx, val, ectx, program)) {
      return;
    }
    ectx.stack.push_back(val);
  }
  void runSub(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {

    auto right = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto left = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto val = ctx->sub(left, right);
    if (checkException(ctx, val, ectx, program)) {
      return;
    }
    ectx.stack.push_back(val);
  }
  void runDiv(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {

    auto right = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto left = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto val = ctx->div(left, right);
    if (checkException(ctx, val, ectx, program)) {
      return;
    }
    ectx.stack.push_back(val);
  }
  void runMul(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {

    auto right = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto left = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto val = ctx->mul(left, right);
    if (checkException(ctx, val, ectx, program)) {
      return;
    }
    ectx.stack.push_back(val);
  }
  void runMod(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {

    auto right = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto left = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto val = ctx->mod(left, right);
    if (checkException(ctx, val, ectx, program)) {
      return;
    }
    ectx.stack.push_back(val);
  }
  void runPow(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {

    auto right = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto left = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto val = ctx->pow(left, right);
    if (checkException(ctx, val, ectx, program)) {
      return;
    }
    ectx.stack.push_back(val);
  }
  void runAnd(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {

    auto right = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto left = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto val = ctx->and_(left, right);
    if (checkException(ctx, val, ectx, program)) {
      return;
    }
    ectx.stack.push_back(val);
  }
  void runOr(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {

    auto right = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto left = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto val = ctx->or_(left, right);
    if (checkException(ctx, val, ectx, program)) {
      return;
    }
    ectx.stack.push_back(val);
  }
  void runNot(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {

    auto value = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto val = ctx->not_(value);
    if (checkException(ctx, val, ectx, program)) {
      return;
    }
    ectx.stack.push_back(val);
  }
  void runLNot(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto value = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    value = ctx->toBoolean(value);
    if (checkException(ctx, value, ectx, program)) {
      return;
    }
    ectx.stack.push_back(ctx->createBoolean(!ctx->checkedBoolean(value)));
  }
  void runXor(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto right = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto left = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto val = ctx->xor_(left, right);
    if (checkException(ctx, val, ectx, program)) {
      return;
    }
    ectx.stack.push_back(val);
  }
  void runShr(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto right = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto left = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto val = ctx->shr(left, right);
    if (checkException(ctx, val, ectx, program)) {
      return;
    }
    ectx.stack.push_back(val);
  }
  void runShl(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto right = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto left = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto val = ctx->shl(left, right);
    if (checkException(ctx, val, ectx, program)) {
      return;
    }
    ectx.stack.push_back(val);
  }
  void runUshr(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runEq(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto right = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto left = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto val = ctx->isEqual(left, right);
    if (checkException(ctx, val, ectx, program)) {
      return;
    }
    ectx.stack.push_back(val);
  }
  void runSeq(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto right = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto left = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    if (left->getType() == right->getType()) {
      auto val = ctx->isEqual(left, right);
      if (checkException(ctx, val, ectx, program)) {
        return;
      }
      ectx.stack.push_back(val);
    } else {
      ectx.stack.push_back(ctx->createBoolean(false));
    }
  }
  void runNe(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto right = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto left = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto val = ctx->isEqual(left, right);
    if (checkException(ctx, val, ectx, program)) {
      return;
    }
    ectx.stack.push_back(ctx->createBoolean(!ctx->checkedBoolean(val)));
  }
  void runSne(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto right = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto left = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto res = ctx->isEqual(left, right);
    if (left->getType() != right->getType()) {
      ectx.stack.push_back(ctx->createBoolean(true));
    } else {
      if (checkException(ctx, res, ectx, program)) {
        return;
      }
      ectx.stack.push_back(ctx->createBoolean(!ctx->checkedBoolean(res)));
    }
  }
  void runGt(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto right = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto left = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto val = ctx->gt(left, right);
    if (checkException(ctx, val, ectx, program)) {
      return;
    }
    ectx.stack.push_back(val);
  }
  void runLt(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto right = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto left = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto val = ctx->lt(left, right);
    if (checkException(ctx, val, ectx, program)) {
      return;
    }
    ectx.stack.push_back(val);
  }
  void runGe(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto right = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto left = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto val = ctx->ge(left, right);
    if (checkException(ctx, val, ectx, program)) {
      return;
    }
    ectx.stack.push_back(val);
  }
  void runLe(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto right = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto left = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto val = ctx->le(left, right);
    if (checkException(ctx, val, ectx, program)) {
      return;
    }
    ectx.stack.push_back(val);
  }
  void runInc(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto value = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    value = ctx->inc(value);
    if (checkException(ctx, value, ectx, program)) {
      return;
    }
    ectx.stack.push_back(value);
  }
  void runDec(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto value = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    value = ctx->dec(value);
    if (checkException(ctx, value, ectx, program)) {
      return;
    }
    ectx.stack.push_back(value);
  }
  void runUpdateInc(JSContext *ctx, const JSProgram &program,
                    JSEvalContext &ectx) {
    auto value = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto res = ctx->createValue(value);
    value = ctx->inc(value);
    if (checkException(ctx, value, ectx, program)) {
      return;
    }
    ectx.stack.push_back(res);
  }
  void runUpdateDec(JSContext *ctx, const JSProgram &program,
                    JSEvalContext &ectx) {
    auto value = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto res = ctx->createValue(value);
    value = ctx->dec(value);
    if (checkException(ctx, value, ectx, program)) {
      return;
    }
    ectx.stack.push_back(res);
  }
  void runNext(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto iterator = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto next = ctx->getField(iterator, ctx->createString(L"next"));
    if (checkException(ctx, next, ectx, program)) {
      return;
    }
    if (!next->isTypeof<JSCallableType>()) {
      ectx.stack.push_back(ctx->createException(JSException::TYPE::TYPE,
                                                L"variable is not iterable"));
      ectx.pc = program.codes.size();
      return;
    }
    auto res = call(ctx, next, iterator, {},
                    {
                        .position =
                            {
                                .funcname = L"next",
                            },
                    });
    if (checkException(ctx, res, ectx, program)) {
      return;
    }
    auto value = ctx->getField(res, ctx->createString(L"value"));
    if (checkException(ctx, value, ectx, program)) {
      return;
    }
    auto done = ctx->toBoolean(ctx->getField(res, ctx->createString(L"done")));
    if (checkException(ctx, done, ectx, program)) {
      return;
    }
    if (ctx->checkedBoolean(done)) {
      ectx.stack.push_back(ctx->createUndefined());
    } else {
      ectx.stack.push_back(value);
    }
    ectx.stack.push_back(done);
  }
  void runAwaitNext(JSContext *ctx, const JSProgram &program,
                    JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runObjectSpread(JSContext *ctx, const JSProgram &program,
                       JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runArraySpread(JSContext *ctx, const JSProgram &program,
                      JSEvalContext &ectx) {
    auto iterator = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto next = ctx->getField(iterator, ctx->createString(L"next"));
    auto res = call(ctx, next, iterator, {},
                    {
                        .position =
                            {
                                .funcname = L"next",
                            },
                    });
    auto done = ctx->getField(res, ctx->createString(L"done"));
    auto value = ctx->getField(res, ctx->createString(L"value"));
    auto array = ctx->createArray();
    auto index = ctx->createNumber(0);
    done = ctx->toBoolean(done);
    while (!ctx->checkedBoolean(done)) {
      ctx->setField(array, index, value);
      ctx->inc(index);
      res = call(ctx, next, iterator, {},
                 {
                     .position =
                         {
                             .funcname = L"next",
                         },
                 });
      done = ctx->getField(res, ctx->createString(L"done"));
      value = ctx->getField(res, ctx->createString(L"value"));
      done = ctx->toBoolean(done);
    }
    ectx.stack.push_back(array);
  }
  void runSuperCall(JSContext *ctx, const JSProgram &program,
                    JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runThis(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    ectx.stack.push_back(ectx.self);
  }
  void runGetSuperField(JSContext *ctx, const JSProgram &program,
                        JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runSetSuperField(JSContext *ctx, const JSProgram &program,
                        JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runArgumentSpread(JSContext *ctx, const JSProgram &program,
                         JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runHlt(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    if (ectx.stack.empty()) {
      ectx.stack.push_back(ctx->createUndefined());
    }
    ectx.pc = program.codes.size();
  }
  void runDebugger(JSContext *ctx, const JSProgram &program,
                   JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runSpread(JSContext *ctx, const JSProgram &program,
                 JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runMerge(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runGetKeys(JSContext *ctx, const JSProgram &program,
                  JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runTryBegin(JSContext *ctx, const JSProgram &program,
                   JSEvalContext &ectx) {
    ectx.tryFrames.push_back({
        .scope = ctx->getScope(),
        .onfinish = 0,
        .onerror = 0,
    });
  }
  void runTryEnd(JSContext *ctx, const JSProgram &program,
                 JSEvalContext &ectx) {
    if (ectx.tryFrames.rbegin()->onfinish != 0) {
      ectx.defer.push_back(ectx.pc);
      ectx.pc = ectx.tryFrames.rbegin()->onfinish;
    }
    ectx.tryFrames.pop_back();
  }
  void runOnFinish(JSContext *ctx, const JSProgram &program,
                   JSEvalContext &ectx) {
    auto onfinish = getAddress(program, ectx.pc);
    ectx.tryFrames.rbegin()->onfinish = onfinish;
  }

  void runDefer(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    if (ectx.result) {
      ectx.stack.push_back(ectx.result);
      ectx.result = nullptr;
    } else {
      ectx.stack.push_back(ctx->createUndefined());
    }
    ectx.pc = *ectx.defer.rbegin();
    ectx.defer.pop_back();
  }

  void runOnError(JSContext *ctx, const JSProgram &program,
                  JSEvalContext &ectx) {
    auto onerror = getAddress(program, ectx.pc);
    ectx.tryFrames.rbegin()->onerror = onerror;
  }

  void runThrow(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto val = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    ectx.stack.push_back(ctx->createException(val));
    ectx.pc = program.codes.size();
  }

  void runEmptyCheck(JSContext *ctx, const JSProgram &program,
                     JSEvalContext &ectx) {
    if (ectx.stack.empty()) {
      ectx.stack.push_back(ctx->createUndefined());
    }
  }

  void runClass(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }

  void runWith(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }

  void runSetInitializer(JSContext *ctx, const JSProgram &program,
                         JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }

  void runImport(JSContext *ctx, const JSProgram &program,
                 JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }

  void runExport(JSContext *ctx, const JSProgram &program,
                 JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }

  void runExportAll(JSContext *ctx, const JSProgram &program,
                    JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }

  void runAssert(JSContext *ctx, const JSProgram &program,
                 JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }

  void runIterator(JSContext *ctx, const JSProgram &program,
                   JSEvalContext &ectx) {
    auto value = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto iterator = ctx->getField(ctx->getSymbolConstructor(),
                                  ctx->createString(L"iterator"));
    iterator = ctx->getField(value, iterator);
    if (checkException(ctx, iterator, ectx, program)) {
      return;
    }
    if (!iterator->isTypeof<JSCallableType>()) {
      ectx.stack.push_back(ctx->createException(JSException::TYPE::TYPE,
                                                L"variable is not iterable"));
      ectx.pc = program.codes.size();
      return;
    }
    auto func = iterator->getData()->cast<JSCallable>();
    auto funcname = func->getName();
    iterator = call(ctx, iterator, value, {},
                    {
                        .position =
                            {
                                .funcname = funcname,
                            },
                        .filename = program.filename,
                    });
    if (checkException(ctx, iterator, ectx, program)) {
      return;
    }
    ectx.stack.push_back(iterator);
  }

  void runSetPrivateField(JSContext *ctx, const JSProgram &program,
                          JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }

  void runGetPrivateField(JSContext *ctx, const JSProgram &program,
                          JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }

  void runSetPrivateMethod(JSContext *ctx, const JSProgram &program,
                           JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }

  void runSetPrivateAccessorGetter(JSContext *ctx, const JSProgram &program,
                                   JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runSetPrivateAccessorSetter(JSContext *ctx, const JSProgram &program,
                                   JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runSetPrivatePropField(JSContext *ctx, const JSProgram &program,
                              JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }

  void runSetPrivatePropMethod(JSContext *ctx, const JSProgram &program,
                               JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }

  void runSetPrivatePropAccessorGetter(JSContext *ctx, const JSProgram &program,
                                       JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runSetPrivatePropAccessorSetter(JSContext *ctx, const JSProgram &program,
                                       JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runSetPrivateInitializer(JSContext *ctx, const JSProgram &program,
                                JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runSetFunctionName(JSContext *ctx, const JSProgram &program,
                          JSEvalContext &ectx) {
    auto name = getString(program, ectx.pc);
    auto func = *ectx.stack.rbegin();
    dynamic_cast<JSCallable *>(func->getData())->setName(name);
  }

  void runBreakLabelBegin(JSContext *ctx, const JSProgram &program,
                          JSEvalContext &ectx) {
    auto label = getString(program, ectx.pc);
    ectx.labels.push_back({
        .type = JSLabelFrame::TYPE::BREAK,
        .label = label,
        .address = 0,
        .scope = ctx->getScope(),
        .tryFrame =
            ectx.tryFrames.empty() ? nullptr : &*ectx.tryFrames.rbegin(),
    });
  }
  void runContinueLabelBegin(JSContext *ctx, const JSProgram &program,
                             JSEvalContext &ectx) {
    auto label = getString(program, ectx.pc);
    ectx.labels.push_back({
        .type = JSLabelFrame::TYPE::CONTINUE,
        .label = label,
        .address = 0,
        .scope = ctx->getScope(),
        .tryFrame =
            ectx.tryFrames.empty() ? nullptr : &*ectx.tryFrames.rbegin(),
    });
  }
  void runLabelEnd(JSContext *ctx, const JSProgram &program,
                   JSEvalContext &ectx) {
    auto &label = *ectx.labels.rbegin();
    std::vector<size_t> defer;
    ectx.defer.push_back(ectx.pc);
    if (label.tryFrame) {
      while (&*ectx.tryFrames.rbegin() != label.tryFrame) {
        auto frame = *ectx.tryFrames.rbegin();
        if (frame.onfinish) {
          defer.push_back(frame.onfinish);
        }
        ectx.tryFrames.pop_back();
      }
    } else {
      while (!ectx.tryFrames.empty()) {
        auto frame = *ectx.tryFrames.rbegin();
        if (frame.onfinish) {
          defer.push_back(frame.onfinish);
        }
        ectx.tryFrames.pop_back();
      }
    }
    for (auto it = defer.rbegin(); it != defer.rend(); it++) {
      ectx.defer.push_back(*it);
    }
    size_t top = 0;
    while (ctx->getScope() != label.scope) {
      ctx->popScope();
      top = *ectx.frames.rbegin();
      ectx.frames.pop_back();
    }
    ectx.stack.resize(top);
    ectx.labels.pop_back();
    ectx.pc = *ectx.defer.rbegin();
    ectx.defer.pop_back();
  }

  void runBreak(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto name = getString(program, ectx.pc);
    for (auto label = ectx.labels.rbegin(); label != ectx.labels.rend();
         label++) {
      if (label->label == name && label->type == JSLabelFrame::TYPE::BREAK) {
        ectx.pc = label->address;
        return;
      }
    }
    ectx.stack.push_back(
        ctx->createException(JSException::TYPE::SYNTAX, L"Invalid label"));
    ectx.pc = program.codes.size();
  }

  void runContinue(JSContext *ctx, const JSProgram &program,
                   JSEvalContext &ectx) {
    auto name = getString(program, ectx.pc);
    for (auto label = ectx.labels.rbegin(); label != ectx.labels.rend();
         label++) {
      if (label->label == name && label->type == JSLabelFrame::TYPE::CONTINUE) {
        ectx.pc = label->address;
        return;
      }
    }
    ectx.stack.push_back(
        ctx->createException(JSException::TYPE::SYNTAX, L"Invalid label"));
    ectx.pc = program.codes.size();
  }

  void runSetLabelAddress(JSContext *ctx, const JSProgram &program,
                          JSEvalContext &ectx) {
    auto address = getAddress(program, ectx.pc);
    ectx.labels.rbegin()->address = address;
  }

  void runOperator(JSContext *ctx, const JSProgram &program,
                   JSEvalContext &ectx, const JS_OPERATOR &opt) {
    switch (opt) {
    case JS_OPERATOR::BEGIN:
      runBegin(ctx, program, ectx);
      break;
    case JS_OPERATOR::END:
      runEnd(ctx, program, ectx);
      break;
    case JS_OPERATOR::PUSH:
      runPush(ctx, program, ectx);
      break;
    case JS_OPERATOR::POP:
      runPop(ctx, program, ectx);
      break;
    case JS_OPERATOR::PUSH_VALUE:
      runPushValue(ctx, program, ectx);
      break;
    case JS_OPERATOR::NIL:
      runNil(ctx, program, ectx);
      break;
    case JS_OPERATOR::UNDEFINED:
      runUndefined(ctx, program, ectx);
      break;
    case JS_OPERATOR::TRUE:
      runTrue(ctx, program, ectx);
      break;
    case JS_OPERATOR::FALSE:
      runFalse(ctx, program, ectx);
      break;
    case JS_OPERATOR::REGEX:
      runRegex(ctx, program, ectx);
      break;
    case JS_OPERATOR::CLASS:
      runClass(ctx, program, ectx);
      break;
    case JS_OPERATOR::BIGINT:
      runBigint(ctx, program, ectx);
      break;
    case JS_OPERATOR::LOAD:
      runLoad(ctx, program, ectx);
      break;
    case JS_OPERATOR::STORE:
      runStore(ctx, program, ectx);
      break;
    case JS_OPERATOR::REF:
      runRef(ctx, program, ectx);
      break;
    case JS_OPERATOR::STR:
      runStr(ctx, program, ectx);
      break;
    case JS_OPERATOR::VAR:
      runVar(ctx, program, ectx);
      break;
    case JS_OPERATOR::CONST:
      runConst(ctx, program, ectx);
      break;
    case JS_OPERATOR::LET:
      runLet(ctx, program, ectx);
      break;
    case JS_OPERATOR::OBJECT:
      runObject(ctx, program, ectx);
      break;
    case JS_OPERATOR::ARRAY:
      runArray(ctx, program, ectx);
      break;
    case JS_OPERATOR::ARROW:
      runArrow(ctx, program, ectx);
      break;
    case JS_OPERATOR::ASYNCARROW:
      runAsyncArrow(ctx, program, ectx);
      break;
    case JS_OPERATOR::FUNCTION:
      runFunction(ctx, program, ectx);
      break;
    case JS_OPERATOR::ASYNCFUNCTION:
      runAsyncfunction(ctx, program, ectx);
      break;
    case JS_OPERATOR::GENERATOR:
      runGenerator(ctx, program, ectx);
      break;
    case JS_OPERATOR::ASYNCGENERATOR:
      runAsyncgenerator(ctx, program, ectx);
      break;
    case JS_OPERATOR::ENABLE:
      runEnable(ctx, program, ectx);
      break;
    case JS_OPERATOR::DISABLE:
      runDisable(ctx, program, ectx);
      break;
    case JS_OPERATOR::GET_FIELD:
      runGetField(ctx, program, ectx);
      break;
    case JS_OPERATOR::SET_FIELD:
      runSetField(ctx, program, ectx);
      break;
    case JS_OPERATOR::SET_METHOD:
      runSetMethod(ctx, program, ectx);
      break;
    case JS_OPERATOR::SET_ACCESSOR_GETTER:
      runSetAccessorGetter(ctx, program, ectx);
      break;
    case JS_OPERATOR::SET_ACCESSOR_SETTER:
      runSetAccessorSetter(ctx, program, ectx);
      break;
    case JS_OPERATOR::SET_PROP_FIELD:
      runSetPropField(ctx, program, ectx);
      break;
    case JS_OPERATOR::SET_PROP_METHOD:
      runSetPropMethod(ctx, program, ectx);
      break;
    case JS_OPERATOR::SET_PROP_ACCESSOR_GETTER:
      runSetPropAccessorGetter(ctx, program, ectx);
      break;
    case JS_OPERATOR::SET_PROP_ACCESSOR_SETTER:
      runSetPropAccessorSetter(ctx, program, ectx);
      break;
    case JS_OPERATOR::SET_INITIALIZER:
      runSetInitializer(ctx, program, ectx);
      break;
    case JS_OPERATOR::GET_PRIVATE_FIELD:
      runGetPrivateField(ctx, program, ectx);
      break;
    case JS_OPERATOR::SET_PRIVATE_FIELD:
      runSetPrivateField(ctx, program, ectx);
      break;
    case JS_OPERATOR::SET_PRIVATE_ACCESSOR_GETTER:
      runSetPrivateAccessorGetter(ctx, program, ectx);
      break;
    case JS_OPERATOR::SET_PRIVATE_ACCESSOR_SETTER:
      runSetPrivateAccessorSetter(ctx, program, ectx);
      break;
    case JS_OPERATOR::SET_PRIVATE_METHOD:
      runSetPrivateMethod(ctx, program, ectx);
      break;
    case JS_OPERATOR::SET_PRIVATE_PROP_FIELD:
      runSetPrivatePropField(ctx, program, ectx);
      break;
    case JS_OPERATOR::SET_PRIVATE_PROP_ACCESSOR_GETTER:
      runSetPrivatePropAccessorGetter(ctx, program, ectx);
      break;
    case JS_OPERATOR::SET_PRIVATE_PROP_ACCESSOR_SETTER:
      runSetPrivatePropAccessorSetter(ctx, program, ectx);
      break;
    case JS_OPERATOR::SET_PRIVATE_PROP_METHOD:
      runSetPrivatePropMethod(ctx, program, ectx);
      break;
    case JS_OPERATOR::SET_PRIVATE_INITIALIZER:
      runSetPrivateInitializer(ctx, program, ectx);
      break;
    case JS_OPERATOR::CALL:
      runCall(ctx, program, ectx);
      break;
    case JS_OPERATOR::MEMBER_CALL:
      runMemberCall(ctx, program, ectx);
      break;
    case JS_OPERATOR::PRIVATE_MEMBER_CALL:
      runPrivateMemberCall(ctx, program, ectx);
      break;
    case JS_OPERATOR::VOID:
      runVoid(ctx, program, ectx);
      break;
    case JS_OPERATOR::TYPEOF:
      runTypeof(ctx, program, ectx);
      break;
    case JS_OPERATOR::NEW:
      runNew(ctx, program, ectx);
      break;
    case JS_OPERATOR::DELETE:
      runDelete(ctx, program, ectx);
      break;
    case JS_OPERATOR::RET:
      runRet(ctx, program, ectx);
      break;
    case JS_OPERATOR::YIELD:
      runYield(ctx, program, ectx);
      break;
    case JS_OPERATOR::AWAIT:
      runAwait(ctx, program, ectx);
      break;
    case JS_OPERATOR::YIELD_DELEGATE:
      runYieldDelegate(ctx, program, ectx);
      break;
    case JS_OPERATOR::JMP:
      runJmp(ctx, program, ectx);
      break;
    case JS_OPERATOR::JTRUE:
      runJtrue(ctx, program, ectx);
      break;
    case JS_OPERATOR::JFALSE:
      runJfalse(ctx, program, ectx);
      break;
    case JS_OPERATOR::JNULL:
      runJnull(ctx, program, ectx);
      break;
    case JS_OPERATOR::JNOT_NULL:
      runJnotNull(ctx, program, ectx);
      break;
    case JS_OPERATOR::UPLUS:
      runUnaryPlus(ctx, program, ectx);
      break;
    case JS_OPERATOR::UNEG:
      runUnaryNegative(ctx, program, ectx);
      break;
    case JS_OPERATOR::ADD:
      runAdd(ctx, program, ectx);
      break;
    case JS_OPERATOR::SUB:
      runSub(ctx, program, ectx);
      break;
    case JS_OPERATOR::DIV:
      runDiv(ctx, program, ectx);
      break;
    case JS_OPERATOR::MUL:
      runMul(ctx, program, ectx);
      break;
    case JS_OPERATOR::MOD:
      runMod(ctx, program, ectx);
      break;
    case JS_OPERATOR::POW:
      runPow(ctx, program, ectx);
      break;
    case JS_OPERATOR::AND:
      runAnd(ctx, program, ectx);
      break;
    case JS_OPERATOR::OR:
      runOr(ctx, program, ectx);
      break;
    case JS_OPERATOR::NOT:
      runNot(ctx, program, ectx);
      break;
    case JS_OPERATOR::LNOT:
      runLNot(ctx, program, ectx);
      break;
    case JS_OPERATOR::XOR:
      runXor(ctx, program, ectx);
      break;
    case JS_OPERATOR::SHR:
      runShr(ctx, program, ectx);
      break;
    case JS_OPERATOR::SHL:
      runShl(ctx, program, ectx);
      break;
    case JS_OPERATOR::USHR:
      runUshr(ctx, program, ectx);
      break;
    case JS_OPERATOR::EQ:
      runEq(ctx, program, ectx);
      break;
    case JS_OPERATOR::SEQ:
      runSeq(ctx, program, ectx);
      break;
    case JS_OPERATOR::NE:
      runNe(ctx, program, ectx);
      break;
    case JS_OPERATOR::SNE:
      runSne(ctx, program, ectx);
      break;
    case JS_OPERATOR::GT:
      runGt(ctx, program, ectx);
      break;
    case JS_OPERATOR::LT:
      runLt(ctx, program, ectx);
      break;
    case JS_OPERATOR::GE:
      runGe(ctx, program, ectx);
      break;
    case JS_OPERATOR::LE:
      runLe(ctx, program, ectx);
      break;
    case JS_OPERATOR::INC:
      runInc(ctx, program, ectx);
      break;
    case JS_OPERATOR::DEC:
      runDec(ctx, program, ectx);
      break;
    case JS_OPERATOR::UPDATE_INC:
      runUpdateInc(ctx, program, ectx);
      break;
    case JS_OPERATOR::UPDATE_DEC:
      runUpdateDec(ctx, program, ectx);
      break;
    case JS_OPERATOR::NEXT:
      runNext(ctx, program, ectx);
      break;
    case JS_OPERATOR::AWAIT_NEXT:
      runAwaitNext(ctx, program, ectx);
      break;
    case JS_OPERATOR::OBJECT_SPREAD:
      runObjectSpread(ctx, program, ectx);
      break;
    case JS_OPERATOR::ARRAY_SPREAD:
      runArraySpread(ctx, program, ectx);
      break;
    case JS_OPERATOR::ARGUMENT_SPREAD:
      runArgumentSpread(ctx, program, ectx);
      break;
    case JS_OPERATOR::HLT:
      runHlt(ctx, program, ectx);
      break;
    case JS_OPERATOR::THIS:
      runThis(ctx, program, ectx);
      break;
    case JS_OPERATOR::SUPER_CALL:
      runSuperCall(ctx, program, ectx);
      break;
    case JS_OPERATOR::DEBUGGER:
      runDebugger(ctx, program, ectx);
      break;
    case JS_OPERATOR::SET_SUPER_FIELD:
      runSetSuperField(ctx, program, ectx);
      break;
    case JS_OPERATOR::GET_SUPER_FIELD:
      runGetSuperField(ctx, program, ectx);
      break;
    case JS_OPERATOR::SPREAD:
      runSpread(ctx, program, ectx);
      break;
    case JS_OPERATOR::MERGE:
      runMerge(ctx, program, ectx);
      break;
    case JS_OPERATOR::GET_KEYS:
      runGetKeys(ctx, program, ectx);
      break;
    case JS_OPERATOR::TRY_BEGIN:
      runTryBegin(ctx, program, ectx);
      break;
    case JS_OPERATOR::TRY_END:
      runTryEnd(ctx, program, ectx);
      break;
    case JS_OPERATOR::DEFER:
      runDefer(ctx, program, ectx);
      break;
    case JS_OPERATOR::ON_FINISH:
      runOnFinish(ctx, program, ectx);
      break;
    case JS_OPERATOR::ON_ERROR:
      runOnError(ctx, program, ectx);
      break;
    case JS_OPERATOR::BREAK:
      runBreak(ctx, program, ectx);
      break;
    case JS_OPERATOR::CONTINUE:
      runContinue(ctx, program, ectx);
      break;
    case JS_OPERATOR::BREAK_LABEL_BEGIN:
      runBreakLabelBegin(ctx, program, ectx);
      break;
    case JS_OPERATOR::CONTINUE_LABEL_BEGIN:
      runContinueLabelBegin(ctx, program, ectx);
      break;
    case JS_OPERATOR::SET_LABELE_ADDRESS:
      runSetLabelAddress(ctx, program, ectx);
      break;
    case JS_OPERATOR::LABEL_END:
      runLabelEnd(ctx, program, ectx);
      break;
    case JS_OPERATOR::THROW:
      runThrow(ctx, program, ectx);
      break;
    case JS_OPERATOR::EMPTY_CHECK:
      runEmptyCheck(ctx, program, ectx);
      break;
    case JS_OPERATOR::ITERATOR:
      runIterator(ctx, program, ectx);
      break;
    case JS_OPERATOR::WITH:
      runWith(ctx, program, ectx);
      break;
    case JS_OPERATOR::IMPORT:
      runImport(ctx, program, ectx);
      break;
    case JS_OPERATOR::EXPORT:
      runExport(ctx, program, ectx);
      break;
    case JS_OPERATOR::ASSERT:
      runAssert(ctx, program, ectx);
      break;
    case JS_OPERATOR::EXPORT_ALL:
      runExportAll(ctx, program, ectx);
      break;
    case JS_OPERATOR::SET_FUNCTION_NAME:
      runSetFunctionName(ctx, program, ectx);
      break;
    }
  }

  JSValue *run(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    JSValue *result = nullptr;
    for (;;) {
      if (ectx.pc >= program.codes.size()) {
        if (!ectx.stack.empty()) {
          result = *ectx.stack.rbegin();
          ectx.stack.pop_back();
        } else {
          result = ctx->createUndefined();
        }
        if (result->isTypeof<JSInterruptType>()) {
          break;
        }
        while (!ectx.tryFrames.empty()) {
          auto frame = *ectx.tryFrames.rbegin();
          result = frame.scope->createValue(result->getAtom());
          while (ctx->getScope() != frame.scope) {
            ctx->popScope();
            auto top = *ectx.frames.rbegin();
            ectx.frames.pop_back();
            ectx.stack.resize(top);
          }
          if (result->isTypeof<JSExceptionType>() && frame.onerror) {
            break;
          }
          if (frame.onfinish != 0) {
            break;
          }
          ectx.tryFrames.pop_back();
        }
        if (!ectx.tryFrames.empty()) {
          auto &frame = *ectx.tryFrames.rbegin();
          if (result->isTypeof<JSExceptionType>() && frame.onerror != 0) {
            ectx.pc = frame.onerror;
            frame.onerror = 0;
            ectx.stack.push_back(ctx->createValue(
                result->getData()->cast<JSException>()->getValue()));
            result = nullptr;
            if (frame.onfinish == 0) {
              ectx.tryFrames.pop_back();
            }
          } else {
            ectx.pc = frame.onfinish;
            ectx.tryFrames.pop_back();
            ectx.defer.push_back(program.codes.size());
            ectx.result = result;
          }
        } else {
          break;
        }
      }
      auto opt = (JS_OPERATOR)(program.codes[ectx.pc]);
      ectx.pc++;
      runOperator(ctx, program, ectx, opt);
    }
    return result;
  }

public:
  JSVirtualMachine(JSAllocator *allocator) : _allocator(allocator) {}

  virtual ~JSVirtualMachine() {}

  JSAllocator *getAllocator() { return _allocator; }

  JSValue *eval(JSContext *ctx, const JSProgram &program,
                JSEvalContext ectx = {}) {
    if (!ectx.self) {
      ectx.self = ctx->createUndefined();
    }
    return run(ctx, program, ectx);
  }
};