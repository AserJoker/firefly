#pragma once
#include "../compiler/JSProgram.hpp"
#include "../util/JSAllocator.hpp"
#include "JSContext.hpp"
#include "JSEvalContext.hpp"
#include "JSExceptionType.hpp"
#include "JSValue.hpp"
#include "script/engine/JSEvalContext.hpp"
#include "script/engine/JSNullType.hpp"
#include "script/util/JSSingleton.hpp"
#include <cstdint>

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
  }
  void runEnd(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
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
    // not implement
    ectx.pc = program.codes.size();
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
    if (result->getType() == JSSingleton::query<JSExceptionType>()) {
      ectx.stack.push_back(result);
      ectx.pc = program.codes.size();
      return;
    }
    ectx.stack.push_back(result);
  }
  void runSetGetter(JSContext *ctx, const JSProgram &program,
                    JSEvalContext &ectx) {}
  void runSetSetter(JSContext *ctx, const JSProgram &program,
                    JSEvalContext &ectx) {}
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
    // not implement
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
    // not implement
    ectx.pc = program.codes.size();
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
    // not implement
    ectx.pc = program.codes.size();
  }
  void runDec(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runUpdateInc(JSContext *ctx, const JSProgram &program,
                    JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runUpdateDec(JSContext *ctx, const JSProgram &program,
                    JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runNext(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
    return;
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
    // not implement
    ectx.pc = program.codes.size();
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
    // not implement
    ectx.pc = program.codes.size();
  }
  void runTryEnd(JSContext *ctx, const JSProgram &program,
                 JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runDefer(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }

  void runThrow(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
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
    // not implement
    ectx.pc = program.codes.size();
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

  void runSetPrivateInitializer(JSContext *ctx, const JSProgram &program,
                                JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }

  void runSetMethod(JSContext *ctx, const JSProgram &program,
                    JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }

  void runSetAccessorGetter(JSContext *ctx, const JSProgram &program,
                            JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runSetAccessorSetter(JSContext *ctx, const JSProgram &program,
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

  void runSetFunctionName(JSContext *ctx, const JSProgram &program,
                          JSEvalContext &ectx) {
    auto name = getString(program, ectx.pc);
    auto func = *ectx.stack.rbegin();
    dynamic_cast<JSCallable *>(func->getData())->setName(name);
  }

  JSValue *run(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    for (;;) {
      if (ectx.pc >= program.codes.size()) {
        break;
      }
      auto opt = (JS_OPERATOR)(program.codes[ectx.pc]);
      ectx.pc++;
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
      case JS_OPERATOR::GET_PRIVATE_FIELD:
        runGetPrivateField(ctx, program, ectx);
        break;
      case JS_OPERATOR::SET_PRIVATE_FIELD:
        runSetPrivateField(ctx, program, ectx);
        break;
      case JS_OPERATOR::SET_INITIALIZER:
        runSetInitializer(ctx, program, ectx);
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
      case JS_OPERATOR::THROW:
        runThrow(ctx, program, ectx);
        break;
      case JS_OPERATOR::EMPTY_CHECK:
        runEmptyCheck(ctx, program, ectx);
        break;
      case JS_OPERATOR::ITERATOR:
        runIterator(ctx, program, ectx);
        break;
      case JS_OPERATOR::SET_GETTER:
        runSetGetter(ctx, program, ectx);
        break;
      case JS_OPERATOR::SET_SETTER:
        runSetSetter(ctx, program, ectx);
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
      case JS_OPERATOR::SET_PRIVATE_INITIALIZER:
        runSetPrivateInitializer(ctx, program, ectx);
        break;
      case JS_OPERATOR::SET_ACCESSOR_GETTER:
        runSetAccessorGetter(ctx, program, ectx);
        break;
      case JS_OPERATOR::SET_ACCESSOR_SETTER:
        runSetAccessorSetter(ctx, program, ectx);
        break;
      case JS_OPERATOR::SET_METHOD:
        runSetMethod(ctx, program, ectx);
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
      case JS_OPERATOR::SET_FUNCTION_NAME:
        runSetFunctionName(ctx, program, ectx);
        break;
      }
    }
    if (ectx.stack.empty()) {
      return ctx->createUndefined();
    }
    return *ectx.stack.rbegin();
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