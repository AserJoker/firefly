#include "script/engine/JSContext.hpp"
#include "script/engine/JSArray.hpp"
#include "script/engine/JSBoolean.hpp"
#include "script/engine/JSBooleanType.hpp"
#include "script/engine/JSCallable.hpp"
#include "script/engine/JSCallableType.hpp"
#include "script/engine/JSException.hpp"
#include "script/engine/JSExceptionType.hpp"
#include "script/engine/JSFunction.hpp"
#include "script/engine/JSNaN.hpp"
#include "script/engine/JSNaNType.hpp"
#include "script/engine/JSNativeFunction.hpp"
#include "script/engine/JSNull.hpp"
#include "script/engine/JSNumber.hpp"
#include "script/engine/JSObject.hpp"
#include "script/engine/JSObjectType.hpp"
#include "script/engine/JSString.hpp"
#include "script/engine/JSStringType.hpp"
#include "script/engine/JSSymbol.hpp"
#include "script/engine/JSUndefined.hpp"
#include "script/engine/JSUninitialize.hpp"
#include "script/engine/JSUninitializeType.hpp"
#include "script/engine/JSValue.hpp"
#include "script/engine/JSVirtualMachine.hpp"
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
  if (_global->isTypeof<JSExceptionType>()) {
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
  auto err = setField(_global, createString(L"global"), _global);
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
  if (value->isTypeof<JSUninitializeType>()) {
    return createException(JSException::TYPE::REFERENCE,
                           L"Cannot access variable before initialization");
  }
  return nullptr;
}

JSValue *JSContext::createValue(JSValue *value) {
  return _current->createValue(value->getAtom());
}
JSValue *JSContext::createValue(JSAtom *atom) {
  return _current->createValue(atom);
}

JSValue *JSContext::clone(JSValue *value) {
  auto type = value->getType();
  return type->clone(this, value);
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

JSValue *JSContext::createNaN() {
  return _current->createValue(_runtime->getAllocator()->create<JSNaN>());
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
  if (!prototype) {
    prototype = createNull();
  }
  auto object = _current->createValue(getAllocator()->create<JSObject>());
  auto obj = object->getData()->cast<JSObject>();
  obj->setPrototype(prototype->getAtom());
  object->getAtom()->addChild(prototype->getAtom());
  return object;
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
  auto val = _current->createValue(
      getAllocator()->create<JSNativeFunction>(name, func, clo));
  for (auto &[_, atom] : clo) {
    val->getAtom()->addChild(atom);
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
  auto &fields = _global->getData()->cast<JSObject>()->getFields();
  for (auto &[atomKey, _] : fields) {
    if (atomKey->isTypeof<JSStringType>()) {
      return getField(_global, createString(name));
    }
  }
  return createException(JSException::TYPE::SYNTAX,
                         std::format(L"'{}' is not defined", name));
}

JSValue *JSContext::assigmentValue(JSValue *variable, JSValue *value) {

  return value;
}

JSValue *JSContext::call(JSValue *func, JSValue *self,
                         const std::vector<JSValue *> args) {
  auto type = func->getType()->cast<JSCallableType>();
  if (!type) {
    return createException(JSException::TYPE::TYPE,
                           L"variable is not a function");
  }
  auto current = _current;
  auto fn = func->getData()->cast<JSCallable>();
  pushScope();
  auto &closures = fn->getClosure();
  for (auto &[name, atom] : closures) {
    auto val = _current->createValue(atom);
    _current->storeValue(name, val);
  }
  auto res = type->call(this, func, self, args);
  auto result = current->createValue(res->getAtom());
  popScope();
  return result;
}

JSValue *JSContext::getGlobal(const std::wstring &name) {
  auto &fields = dynamic_cast<JSObject *>(_global->getData())->getFields();
  for (auto &[keyAtom, value] : fields) {
    if (keyAtom->isTypeof<JSStringType>()) {
      if (keyAtom->getData()->cast<JSString>()->getValue() == name) {
        return getField(_global, createString(name));
      }
    }
  }
  return nullptr;
}

JSValue *JSContext::setField(JSValue *obj, JSValue *name, JSValue *value) {
  auto type = obj->getType();
  auto object = type->pack(this, obj);
  if (object->isTypeof<JSExceptionType>()) {
    return object;
  }
  auto otype = object->getType()->cast<JSObjectType>();
  return otype->setField(this, object, name, value);
}

JSValue *JSContext::getField(JSValue *obj, JSValue *name) {
  auto type = obj->getType();
  auto object = type->pack(this, obj);
  if (object->isTypeof<JSExceptionType>()) {
    return object;
  }
  auto otype = object->getType()->cast<JSObjectType>();
  return otype->getField(this, object, name);
}

JSValue *JSContext::getKeys(JSValue *obj) {
  auto type = obj->getType();
  auto object = type->pack(this, obj);
  if (object->isTypeof<JSExceptionType>()) {
    return object;
  }
  auto otype = object->getType()->cast<JSObjectType>();
  return otype->getKeys(this, object);
}

JSValue *JSContext::toString(JSValue *value) {
  return value->getType()->toString(this, value);
}

JSValue *JSContext::toNumber(JSValue *value) {
  return value->getType()->toNumber(this, value);
}

JSValue *JSContext::toBoolean(JSValue *value) {
  return value->getType()->toBoolean(this, value);
}

const std::wstring &JSContext::checkedString(JSValue *value) const {
  static std::wstring def = L"";
  if (value->isTypeof<JSStringType>()) {
    return value->getData()->cast<JSString>()->getValue();
  }
  return def;
}

double JSContext::checkedNumber(JSValue *value) const {
  if (value->isTypeof<JSNumberType>()) {
    return value->getData()->cast<JSNumber>()->getValue();
  }
  return 0;
}

bool JSContext::checkedBoolean(JSValue *value) const {
  if (value->isTypeof<JSBooleanType>()) {
    return value->getData()->cast<JSBoolean>()->getValue();
  }
  return false;
}

bool JSContext::isNaN(JSValue *value) const {
  return value->isTypeof<JSNaNType>();
}

JSValue *JSContext::isEqual(JSValue *left, JSValue *right) {
  if (left->isTypeof<JSObjectType>()) {
    left = left->getType()->cast<JSObjectType>()->unpack(this, left);
  }
  if (right->isTypeof<JSObjectType>()) {
    right = right->getType()->cast<JSObjectType>()->unpack(this, right);
  }
  auto type = left->getType();
  if (left->getType()->getPriority() < right->getType()->getPriority()) {
    type = right->getType();
  }
  return type->equal(this, left, right);
}