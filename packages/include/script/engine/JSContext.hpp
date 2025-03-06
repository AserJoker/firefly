#pragma once

#include "JSException.hpp"
#include "JSObject.hpp"
#include "JSRuntime.hpp"
#include "JSScope.hpp"
#include "JSStackFrame.hpp"
#include "script/engine/JSCallable.hpp"
#include "script/engine/JSValue.hpp"

class JSContext {
private:
  JSRuntime *_runtime;

  JSScope *_root;

  JSScope *_current;

  std::wstring _currentPath;

  std::vector<JSStackFrame> _callstacks;

private:
  JSValue *_global;

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

  JSValue *checkUninitialized(JSValue *value);

  JSValue *createValue(JSValue *value);

  JSValue *createValue(JSAtom *atom);

  JSValue *clone(JSValue *value);

  JSValue *createUndefined();

  JSValue *createUninitialized();

  JSValue *createNull();

  JSValue *createNaN();

  JSValue *createSymbol(const std::wstring &description = L"");

  JSValue *createNumber(double value);

  JSValue *createString(const std::wstring &value);

  JSValue *createBoolean(bool value);

  JSValue *createObject(JSValue *prototype = nullptr);

  JSValue *createArray();

  JSValue *createNativeFunction(
      const JS_NATIVE &func, const std::wstring &name,
      const std::unordered_map<std::wstring, JSValue *> &closure = {});

  JSValue *createFunction(
      const std::wstring &name, const std::wstring &filename, size_t address,
      const std::unordered_map<std::wstring, JSValue *> &closure = {});

  JSValue *createException(const JSException::TYPE &type,
                           const std::wstring &message,
                           const std::wstring &filename = L"",
                           size_t column = 0, size_t line = 0);

  JSValue *getGlobal();

  JSValue *queryValue(const std::wstring &name);

  JSValue *assigmentValue(JSValue *variable, JSValue *value);

  JSValue *call(JSValue *func, JSValue *self,
                const std::vector<JSValue *> args);

  JSValue *getGlobal(const std::wstring &name);

  JSValue *setField(JSValue *obj, JSValue *name, JSValue *value);

  JSValue *defineProperty(JSValue *obj, JSValue *name, JSValue *value,
                          bool configurable, bool enumable, bool writable);

  JSValue *defineProperty(JSValue *obj, JSValue *name, JSValue *getter,
                          JSValue *setter, bool configurable, bool enumable);

  JSValue *getField(JSValue *obj, JSValue *name);

  JSValue *getKeys(JSValue *obj);

  JSValue *toString(JSValue *value);

  JSValue *toNumber(JSValue *value);

  JSValue *toBoolean(JSValue *value);

  const std::wstring &checkedString(JSValue *value) const;

  double checkedNumber(JSValue *value) const;

  bool checkedBoolean(JSValue *value) const;

  bool isNaN(JSValue *value) const;

  JSValue *isEqual(JSValue *left, JSValue *right);
};