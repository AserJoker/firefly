#include "script/engine/JSContext.hpp"
#include "script/engine/JSArray.hpp"
#include "script/engine/JSBoolean.hpp"
#include "script/engine/JSFunction.hpp"
#include "script/engine/JSNativeFunction.hpp"
#include "script/engine/JSNull.hpp"
#include "script/engine/JSNumber.hpp"
#include "script/engine/JSString.hpp"
#include "script/engine/JSSymbol.hpp"
#include "script/engine/JSUndefined.hpp"
#include "script/engine/JSUninitialize.hpp"
#include "script/engine/JSValue.hpp"
#include "script/engine/JSVirtualMachine.hpp"
#include "script/runtime/JSFunctionConstructor.hpp"
#include "script/runtime/JSObjectConstructor.hpp"
#include "script/runtime/JSSymbolConstructor.hpp"
#include <fstream>

JSContext::JSContext(JSRuntime *runtime) : _runtime(runtime) {
  _root = getAllocator()->create<JSScope>();
  _current = _root;
  _callstacks.push_back({
      .position =
          {
              .funcname = L"neo.run",
          },
  });
  auto err = initializeGlobal();
  if (err) {
    _global = err;
  }
}

JSContext::~JSContext() {
  _current = nullptr;
  getAllocator()->dispose(_root);
  _root = nullptr;
  _runtime = nullptr;
}

JSValue *JSContext::eval(const std::wstring &filename,
                         const std::wstring &source, const JS_EVAL_TYPE &type) {
  if (_global->getType() == JS_TYPE::EXCEPTION) {
    return _global;
  }
  if (!_runtime->hasProgram(filename)) {
    auto &program = _runtime->compile(filename, source, type);
    if (program.error) {
      auto err = dynamic_cast<JSErrorNode *>(program.error);
      auto exception = createException(
          JSException::TYPE::SYNTAX, err->message, filename,
          program.error->location.end.column, program.error->location.end.line);
      return exception;
    }
  }
  auto &program = _runtime->getProgram(filename);
  if (type == JS_EVAL_TYPE::PROGRAM) {
    std::wfstream out("./1.asm", std::ios_base::out);
    if (!out.is_open()) {
      return createException(JSException::TYPE::ERROR, L"cannot open 1.asm");
    }
    out << program.toString();
    out.close();
  } else {
    std::wfstream out("./2.asm", std::ios_base::out);
    if (!out.is_open()) {
      return createException(JSException::TYPE::ERROR, L"cannot open 1.asm");
    }
    out << program.toString();
    out.close();
  }
  return _runtime->getVirtualMachine()->eval(this, program);
}

JSValue *JSContext::initializeGlobal() {
  _global = createObject(createNull());
  auto err = setField(_global, L"global", _global);
  if (err) {
    return err;
  }
  err = JSFunctionConstructor::initialize(this);
  if (err) {
    return err;
  }
  err = JSObjectConstructor::initialize(this);
  if (err) {
    return err;
  }
  err = JSSymbolConstructor::initialize(this);
  if (err) {
    return err;
  }
  return nullptr;
}

void JSContext::pushCallStack(const std::wstring &filename,
                              const std::wstring &funcname, size_t column,
                              size_t line) {
  _callstacks.rbegin()->position.column = column;
  _callstacks.rbegin()->position.line = line;
  _callstacks.rbegin()->filename = filename;
  _callstacks.push_back({
      .position = {.funcname = funcname},
  });
}

void JSContext::popCallStack() { _callstacks.pop_back(); }

std::vector<JSStackFrame> JSContext::trace(const std::wstring &filename,
                                           size_t column, size_t line) {
  auto result = _callstacks;
  result.rbegin()->filename = filename;
  result.rbegin()->position.column = column;
  result.rbegin()->position.line = line;
  return result;
}

void JSContext::pushScope() {
  _current = getAllocator()->create<JSScope>(_current);
  _runtime->getLogger()->debug(L"push scope: 0x{:x}", (ptrdiff_t)_current);
}
void JSContext::popScope() {
  _runtime->getLogger()->debug(L"pop scope: 0x{:x}", (ptrdiff_t)_current);
  auto parent = _current->getParent();
  parent->removeChild(_current);
  getAllocator()->dispose(_current);
  _current = parent;
}
JSScope *JSContext::setScope(JSScope *scope) {
  auto old = _current;
  _runtime->getLogger()->debug(L"scope change: 0x{:x} -> 0x{:x}",
                               (ptrdiff_t)_current, (ptrdiff_t)scope);
  _current = scope;
  return old;
}
JSScope *JSContext::setRootScope(JSScope *scope) {
  _runtime->getLogger()->debug(L"scope change: 0x{:x} -> 0x{:x}",
                               (ptrdiff_t)_current, (ptrdiff_t)scope);
  auto old = _root;
  _root = scope;
  _current = scope;
  return old;
}

JSValue *JSContext::checkUninitialized(JSValue *value) {
  if (value->getType() == JS_TYPE::UNINITIALIZED) {
    return createException(JSException::TYPE::REFERENCE,
                           L"Cannot access variable before initialization");
  }
  return nullptr;
}
JSValue *JSContext::createUndefined() {
  return _current->createValue(_runtime->getAllocator()->create<JSUndefined>());
}

JSValue *JSContext::createUninitialized() {
  return _current->createValue(
      _runtime->getAllocator()->create<JSUninitialize>());
}

JSValue *JSContext::createNull() {
  return _current->createValue(_runtime->getAllocator()->create<JSNull>());
}

JSValue *JSContext::createSymbol(const std::wstring &description) {
  return _current->createValue(
      _runtime->getAllocator()->create<JSSymbol>(description));
}

JSValue *JSContext::createNumber(double value) {
  return _current->createValue(
      _runtime->getAllocator()->create<JSNumber>(value));
}

JSValue *JSContext::createString(const std::wstring &value) {
  return _current->createValue(
      _runtime->getAllocator()->create<JSString>(value));
}

JSValue *JSContext::createBoolean(bool value) {
  return _current->createValue(
      _runtime->getAllocator()->create<JSBoolean>(value));
}

JSValue *JSContext::createObject(JSValue *prototype) {

  auto obj =
      _current->createValue(_runtime->getAllocator()->create<JSObject>());
  auto object = dynamic_cast<JSObject *>(obj->getData());
  if (!prototype) {
    auto Object = getGlobal(L"Object");
    if (Object && Object->getType() == JS_TYPE::EXCEPTION) {
      return Object;
    }
    if (Object) {
      auto proto = getField(Object, L"prototype");
      if (proto->getType() == JS_TYPE::EXCEPTION) {
        return proto;
      }
      prototype = proto;
      object->setConstructor(Object->getAtom());
      obj->getAtom()->addChild(Object->getAtom());
    }
  }
  if (prototype) {
    object->setPrototype(prototype->getAtom());
    prototype->getAtom()->addChild(obj->getAtom());
  }
  return obj;
}

JSValue *JSContext::createArray() {
  return _current->createValue(getAllocator()->create<JSArray>());
}

JSValue *JSContext::createNativeFunction(
    const JS_NATIVE &func, const std::wstring &name,
    const std::unordered_map<std::wstring, JSValue *> &closure) {
  std::unordered_map<std::wstring, JSAtom *> clo;
  for (auto &[n, val] : closure) {
    clo[n] = val->getAtom();
  }
  auto function =
      _runtime->getAllocator()->create<JSNativeFunction>(name, func, clo);
  auto Function = getGlobal(L"Function");
  if (Function && Function->getType() == JS_TYPE::EXCEPTION) {
    return Function;
  }
  auto prototype = createObject();

  auto proto = getField(Function, L"prototype");
  if (!proto) {
    return createException(JSException::TYPE::TYPE,
                           L"variable is not a constructor");
  }
  auto val = _current->createValue(function);
  function->setPrototype(proto->getAtom());
  val->getAtom()->addChild(proto->getAtom());
  auto err = setField(val, L"constructor", Function);
  if (err) {
    return err;
  }
  for (auto &[n, atom] : clo) {
    val->getAtom()->addChild(atom);
  }
  if (auto err = setField(prototype, L"constructor", val)) {
    return err;
  }
  if (auto err = setField(val, L"prototype", prototype)) {
    return err;
  }
  if (!name.empty()) {
    if (auto err = setField(val, L"name", createString(name))) {
      return err;
    }
  }
  return val;
}

JSValue *JSContext::createFunction(
    const std::wstring &name, const std::wstring &filename, size_t address,
    const std::unordered_map<std::wstring, JSValue *> &closure) {
  std::unordered_map<std::wstring, JSAtom *> clo;
  for (auto &[n, val] : closure) {
    clo[n] = val->getAtom();
  }
  auto val = _current->createValue(_runtime->getAllocator()->create<JSFunction>(
      name, filename, address, clo));
  for (auto &[n, atom] : clo) {
    val->getAtom()->addChild(atom);
  }
  return val;
}

JSValue *JSContext::createException(const JSException::TYPE &type,
                                    const std::wstring &message,
                                    const std::wstring &filename, size_t column,
                                    size_t line) {
  return _current->createValue(_runtime->getAllocator()->create<JSException>(
      type, message, trace(filename, column, line)));
}

JSValue *JSContext::getGlobal() { return _global; }

JSValue *JSContext::queryValue(const std::wstring &name) {
  auto scope = _current;
  while (scope) {
    auto val = scope->queryValue(name);
    if (val) {
      return val;
    }
    scope = scope->getParent();
  }
  auto keys = getKeys(_global);
  if (std::find(keys.begin(), keys.end(), name) == keys.end()) {
    return createException(JSException::TYPE::REFERENCE,
                           name + L" is not defined");
  }
  return getField(_global, name);
}

JSValue *JSContext::assigmentValue(JSValue *variable, JSValue *value) {
  if (variable->isConst() && variable->getType() != JS_TYPE::UNINITIALIZED) {
    return createException(JSException::TYPE::TYPE,
                           L"Assignment to constant variable.");
  }
  _current->getRoot()->addChild(variable->getAtom());
  variable->getAtom()->setData(value->getData()->clone());
  return value;
}

JSValue *JSContext::call(JSValue *func, JSValue *self,
                         const std::vector<JSValue *> args) {
  if (auto err = checkUninitialized(func)) {
    return err;
  }
  if (func->getType() < JS_TYPE::FUNCTION) {
    return createException(JSException::TYPE::TYPE,
                           L"vairable is not a function");
  }
  auto current = getScope();
  auto fn = const_cast<JSCallable *>(
      dynamic_cast<const JSCallable *>(func->getAtom()->getData()));
  if (fn->isGlobalContext()) {
    setScope(_root);
  }
  pushScope();
  for (auto &[key, val] : fn->getClosure()) {
    auto value = _current->createValue(val);
    _current->storeValue(key, value);
  }
  auto res = fn->call(this, self, args);
  if (res->getType() == JS_TYPE::EXCEPTION) {
    return res;
  }
  auto result = current->createValue(res->getAtom());
  popScope();
  if (fn->isGlobalContext()) {
    setScope(current);
  }
  return result;
}

JSValue *JSContext::construct(JSValue *constructor,
                              const std::vector<JSValue *> args) {
  if (auto err = checkUninitialized(constructor)) {
    return err;
  }
  auto obj = createObject();
  obj->getData()->cast<JSObject>()->setConstructor(constructor->getAtom());
  obj->getAtom()->addChild(constructor->getAtom());
  auto res = call(constructor, obj, args);
  if (res->getType() >= JS_TYPE::OBJECT) {
    return res;
  }
  return obj;
}
JSValue *JSContext::construct(JSBase *base, JSValue *constructor,
                              const std::vector<JSValue *> args) {
  if (auto err = checkUninitialized(constructor)) {
    return err;
  }
  if (constructor->getType() == JS_TYPE::FUNCTION) {
    auto prototype = getField(constructor, L"prototype");
    if (!prototype) {
      return createException(JSException::TYPE::TYPE,
                             L"variable is not a constructor");
    }
    auto object = dynamic_cast<JSObject *>(base);
    if (!object) {
      return createException(JSException::TYPE::TYPE,
                             L"variable is not a object");
    }
    object->setPrototype(prototype->getAtom());
    auto obj = _current->createValue(base);
    obj->getAtom()->addChild(prototype->getAtom());
    auto err = setField(obj, L"constructor", constructor);
    if (err) {
      return err;
    }
    auto res = call(constructor, obj, args);
    if (res && res->getType() >= JS_TYPE::OBJECT) {
      return res;
    }
    return obj;
  }
  return createException(JSException::TYPE::TYPE,
                         L"variable is not a constructor");
}

JSValue *JSContext::getGlobal(const std::wstring &name) {
  auto &fields = dynamic_cast<JSObject *>(_global->getData())->getFields();
  if (fields.contains(name)) {
    return getField(_global, name);
  }
  return nullptr;
}

JSValue *JSContext::setField(JSValue *obj, const std::wstring &name,
                             JSValue *value) {
  if (auto err = checkUninitialized(obj)) {
    return err;
  }
  if (auto err = checkUninitialized(value)) {
    return err;
  }
  auto object = dynamic_cast<JSObject *>(obj->getData());
  return object->setField(this, obj, name, value);
}

JSValue *JSContext::getField(JSValue *obj, const std::wstring &name) {
  if (auto err = checkUninitialized(obj)) {
    return err;
  }
  auto object = dynamic_cast<JSObject *>(obj->getData());
  return object->getField(this, obj, name);
}

JSValue *JSContext::setField(JSValue *obj, JSValue *name, JSValue *value) {
  if (auto err = checkUninitialized(obj)) {
    return err;
  }
  if (auto err = checkUninitialized(name)) {
    return err;
  }
  if (auto err = checkUninitialized(value)) {
    return err;
  }
  auto object = dynamic_cast<JSObject *>(obj->getData());
  return object->setField(this, obj, name, value);
}

JSValue *JSContext::getField(JSValue *obj, JSValue *name) {
  if (auto err = checkUninitialized(obj)) {
    return err;
  }
  if (auto err = checkUninitialized(name)) {
    return err;
  }
  auto object = dynamic_cast<JSObject *>(obj->getData());
  return object->getField(this, obj, name);
}

JSValue *JSContext::setIndex(JSValue *obj, size_t index, JSValue *value) {
  if (auto err = checkUninitialized(obj)) {
    return err;
  }
  auto object = dynamic_cast<JSObject *>(obj->getData());
  return object->setIndex(this, obj, index, value);
}

JSValue *JSContext::getIndex(JSValue *obj, size_t index) {
  if (auto err = checkUninitialized(obj)) {
    return err;
  }
  auto object = dynamic_cast<JSObject *>(obj->getData());
  return object->getIndex(this, obj, index);
}

std::vector<std::wstring> JSContext::getKeys(JSValue *obj) {
  auto object = dynamic_cast<JSObject *>(obj->getData());
  return object->getKeys(this);
}

JSValue *JSContext::toString(JSValue *value) {
  if (auto err = checkUninitialized(value)) {
    return err;
  }
  return _current->createValue(value->getData()->toString());
}

const std::wstring &JSContext::checkedString(JSValue *value) const {
  static std::wstring def = L"";
  if (value->getType() == JS_TYPE::STRING) {
    return value->getData()->cast<JSString>()->getValue();
  }
  return def;
}