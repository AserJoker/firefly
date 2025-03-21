#pragma once

#include "JSEvalContext.hpp"
#include "JSException.hpp"
#include "JSObject.hpp"
#include "JSRuntime.hpp"
#include "JSScope.hpp"
#include "JSStackFrame.hpp"
#include "script/engine/JSCallable.hpp"
#include "script/engine/JSExceptionType.hpp"
#include "script/engine/JSValue.hpp"
#include "script/util/BigInt.hpp"
#include <string>

class JSContext {
private:
  JSRuntime *_runtime;

  JSScope *_root;

  JSScope *_current;

  std::wstring _currentPath;

  std::vector<JSStackFrame> _callstacks;

  JSValue *_Object{};

  JSValue *_Function{};

  JSValue *_Symbol{};

  JSValue *_Array{};

  JSValue *_String{};

  JSValue *_Number{};

  JSValue *_Boolean{};

  JSValue *_BigInt{};

  JSValue *_GeneratorFunction{};

  JSValue *_Generator{};

  JSValue *_AsyncFunction{};

  JSValue *_AsyncGeneratorFunction{};

  JSValue *_AsyncGenerator{};

  JSValue *_Promise{};

private:
  JSValue *_global{};

  JSAtom *_classContext{};

public:
  JSContext(JSRuntime *runtime);

  ~JSContext();

  inline JSAllocator *getAllocator() { return _runtime->getAllocator(); }

  JSValue *initializeGlobal();

  void pushCallStack(const std::wstring &filename, const std::wstring &funcname,
                     size_t column, size_t line);

  void popCallStack();

  std::vector<JSStackFrame> trace(const std::wstring &filename, size_t column,
                                  size_t line);

  inline const std::vector<JSStackFrame> &getCallStack() { return _callstacks; }

  inline void setCallStack(const std::vector<JSStackFrame> &stacks) {
    _callstacks = stacks;
  }

  inline JSRuntime *getRuntime() { return _runtime; }

  inline const std::wstring &getCurrentPath() const { return _currentPath; }

  inline void setCurrentPath(const std::wstring &path) { _currentPath = path; }

  JSValue *eval(const std::wstring &filename, const std::wstring &source,
                const JS_EVAL_TYPE &type = JS_EVAL_TYPE::PROGRAM);

  void pushScope();

  void popScope();

  inline JSScope *getScope() { return _current; }

  inline JSScope *getRootScope() { return _root; }

  JSScope *setScope(JSScope *scope);

  JSScope *setRootScope(JSScope *scope);

  inline void recycle(JSValue *val) {
    _current->getRoot()->addChild(val->getAtom());
  }

  inline void recycle(JSAtom *atom) { _current->getRoot()->addChild(atom); }

  inline bool isException(JSValue *value) {
    return value->isTypeof<JSExceptionType>();
  }

  JSValue *isUninitialized(JSValue *value);

  JSValue *createValue(JSValue *value);

  JSValue *createValue(JSAtom *atom);

  JSValue *clone(JSValue *value);

  JSValue *createUndefined();

  JSValue *createUninitialized();

  JSValue *createNull();

  JSValue *createNaN();

  JSValue *createInfinity(bool negative = false);

  JSValue *createBigInt(const BigInt<> &bigint);

  JSValue *createSymbol(const std::wstring &description = L"");

  JSValue *createNumber(double value);

  JSValue *createString(const std::wstring &value);

  JSValue *createBoolean(bool value);

  JSValue *createObject(JSValue *prototype = nullptr);

  JSValue *createArray();

  JSValue *createInterrupt(const JSEvalContext &ectx, JSValue *value);

  JSValue *createNativeFunction(
      const JS_NATIVE &func, const std::wstring &name,
      const std::unordered_map<std::wstring, JSValue *> &closure = {});

  JSValue *createFunction(
      const std::wstring &name, const std::wstring &filename, size_t address,
      const std::unordered_map<std::wstring, JSValue *> &closure = {});

  JSValue *createGeneratorFunction(
      const std::wstring &name, const std::wstring &filename, size_t address,
      const std::unordered_map<std::wstring, JSValue *> &closure = {});

  JSValue *createException(const JSException::TYPE &type,
                           const std::wstring &message,
                           const std::wstring &filename = L"",
                           size_t column = 0, size_t line = 0);

  JSValue *createException(JSValue *error);

  JSValue *getGlobal();

  JSValue *queryValue(const std::wstring &name);

  JSValue *assigmentValue(JSValue *variable, JSValue *value);

  JSValue *call(JSValue *func, JSValue *self,
                const std::vector<JSValue *> args);

  JSValue *construct(JSValue *constructor,
                     const std::vector<JSValue *> &args = {});

  JSValue *getPrototypeOf(JSValue *value);

  JSValue *setPrototype(JSValue *value, JSValue *prototype);

  JSValue *getConstructorOf(JSValue *value);

  JSValue *setConstructor(JSValue *value, JSValue *constructor);

  JSValue *getField(JSValue *obj, JSValue *name);

  JSValue *setField(JSValue *obj, JSValue *name, JSValue *value);

  JSValue *defineProperty(JSValue *obj, JSValue *name, JSValue *value,
                          bool configurable, bool enumable, bool writable);

  JSValue *defineProperty(JSValue *obj, JSValue *name, JSValue *getter,
                          JSValue *setter, bool configurable, bool enumable);

  JSValue *getPrivateField(JSValue *obj, const std::wstring &name);

  JSValue *setPrivateField(JSValue *obj, const std::wstring &name,
                           JSValue *value);

  JSValue *definePrivateProperty(JSValue *obj, const std::wstring &name,
                                 JSValue *value);

  JSValue *definePrivateProperty(JSValue *obj, const std::wstring &name,
                                 JSValue *getter, JSValue *setter);

  JSValue *getKeys(JSValue *obj);

  JSValue *toString(JSValue *value);

  JSValue *toNumber(JSValue *value);

  JSValue *toBoolean(JSValue *value);

  const std::wstring &checkedString(JSValue *value) const;

  double checkedNumber(JSValue *value) const;

  bool checkedBoolean(JSValue *value) const;

  bool isNaN(JSValue *value) const;

  JSValue *unaryNegation(JSValue *left);

  JSValue *unaryPlus(JSValue *left);

  JSValue *isEqual(JSValue *left, JSValue *right);

  JSValue *add(JSValue *left, JSValue *right);

  JSValue *sub(JSValue *value, JSValue *another);

  JSValue *mul(JSValue *value, JSValue *another);

  JSValue *div(JSValue *value, JSValue *another);

  JSValue *mod(JSValue *value, JSValue *another);

  JSValue *pow(JSValue *value, JSValue *another);

  JSValue *and_(JSValue *value, JSValue *another);

  JSValue *or_(JSValue *value, JSValue *another);

  JSValue *xor_(JSValue *value, JSValue *another);

  JSValue *shr(JSValue *value, JSValue *another);

  JSValue *shl(JSValue *value, JSValue *another);

  JSValue *gt(JSValue *value, JSValue *another);

  JSValue *ge(JSValue *value, JSValue *another);

  JSValue *lt(JSValue *value, JSValue *another);

  JSValue *le(JSValue *value, JSValue *another);

  JSValue *not_(JSValue *value);

  JSValue *inc(JSValue *value);

  JSValue *dec(JSValue *value);

  void setMetadata(JSValue *value, const std::wstring &name, JSValue *metadata);

  JSValue *getMetadata(JSValue *value, const std::wstring &name);

  JSValue *pack(JSValue *value);

  JSValue *unpack(JSValue *value);

  JSValue *getObjectConstructor() { return _Object; };

  JSValue *getFunctionConstructor() { return _Function; };

  JSValue *getSymbolConstructor() { return _Symbol; };

  JSValue *getArrayConstructor() { return _Array; };

  JSValue *getStringConstructor() { return _String; };

  JSValue *getNumberConstructor() { return _Number; };

  JSValue *getBooleanConstructor() { return _Boolean; };

  JSValue *getBigIntConstructor() { return _BigInt; };

  JSValue *getGeneratorFunctionConstructor() { return _GeneratorFunction; };

  JSValue *getGeneratorConstructor() { return _Generator; };

  JSValue *getAsyncFunctionConstructor() { return _AsyncFunction; };

  JSValue *getAsyncGeneratorFunctionConstructor() {
    return _AsyncGeneratorFunction;
  };

  JSValue *getAsyncGeneratorConstructor() { return _AsyncGenerator; };

  JSValue *getPromiseConstructor() { return _Promise; };

  JSValue *getCurrentClass() {
    return _classContext ? createValue(_classContext) : nullptr;
  }
  JSValue *setCurrentClass(JSValue *clazz) {
    auto old = _classContext;
    if (clazz) {
      _classContext = clazz->getAtom();
    } else {
      _classContext = nullptr;
    }
    return old ? createValue(old) : nullptr;
  }
};

#define CHECK(ctx, value)                                                      \
  if (value) {                                                                 \
    if (ctx->isUninitialized(value)) {                                         \
      return ctx->createException(JSException::TYPE::SYNTAX,                   \
                                  L"variable is uninitialized");               \
    }                                                                          \
    if (ctx->isException(value)) {                                             \
      return value;                                                            \
    }                                                                          \
  }

#define JS_CFUNCTION(name)                                                     \
  JSValue *name(JSContext *ctx, JSValue *self, std::vector<JSValue *> args)
