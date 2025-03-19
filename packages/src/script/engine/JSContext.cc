#include "script/engine/JSContext.hpp"
#include "script/engine/JSArray.hpp"
#include "script/engine/JSBigInt.hpp"
#include "script/engine/JSBigIntType.hpp"
#include "script/engine/JSBoolean.hpp"
#include "script/engine/JSBooleanType.hpp"
#include "script/engine/JSCallable.hpp"
#include "script/engine/JSCallableType.hpp"
#include "script/engine/JSException.hpp"
#include "script/engine/JSFunction.hpp"
#include "script/engine/JSGenerator.hpp"
#include "script/engine/JSGeneratorFunction.hpp"
#include "script/engine/JSInfinity.hpp"
#include "script/engine/JSInterrupt.hpp"
#include "script/engine/JSNaN.hpp"
#include "script/engine/JSNaNType.hpp"
#include "script/engine/JSNativeFunction.hpp"
#include "script/engine/JSNull.hpp"
#include "script/engine/JSNullType.hpp"
#include "script/engine/JSNumber.hpp"
#include "script/engine/JSNumberType.hpp"
#include "script/engine/JSObject.hpp"
#include "script/engine/JSObjectType.hpp"
#include "script/engine/JSString.hpp"
#include "script/engine/JSStringType.hpp"
#include "script/engine/JSSymbol.hpp"
#include "script/engine/JSSymbolType.hpp"
#include "script/engine/JSUndefined.hpp"
#include "script/engine/JSUndefinedType.hpp"
#include "script/engine/JSUninitialize.hpp"
#include "script/engine/JSUninitializeType.hpp"
#include "script/engine/JSValue.hpp"
#include "script/engine/JSVirtualMachine.hpp"
#include "script/runtime/JSArrayConstructor.hpp"
#include "script/runtime/JSBigIntConstructor.hpp"
#include "script/runtime/JSBooleanConstructor.hpp"
#include "script/runtime/JSFunctionConstructor.hpp"
#include "script/runtime/JSGeneratorConstructor.hpp"
#include "script/runtime/JSGeneratorFunctionConstructor.hpp"
#include "script/runtime/JSNumberConstructor.hpp"
#include "script/runtime/JSObjectConstructor.hpp"
#include "script/runtime/JSStringConstructor.hpp"
#include "script/runtime/JSSymbolConstructor.hpp"
#include <fstream>
#include <iostream>

JSContext::JSContext(JSRuntime *runtime) : _runtime(runtime), _global(nullptr) {
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
  CHECK(this, _global);
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
  CHECK(this, err)
  _Function = JSFunctionConstructor::initialize(this);
  CHECK(this, _Function)
  _Object = JSObjectConstructor::initialize(this);
  CHECK(this, _Object)
  _Symbol = JSSymbolConstructor::initialize(this);
  CHECK(this, _Symbol)
  _Array = JSArrayConstructor::initialize(this);
  CHECK(this, _Array);
  _Boolean = JSBooleanConstructor::initialize(this);
  CHECK(this, _Boolean)
  _Number = JSNumberConstructor::initialize(this);
  CHECK(this, _Number)
  _String = JSStringConstructor::initialize(this);
  CHECK(this, _String)
  _BigInt = JSBigIntConstructor::initialize(this);
  CHECK(this, _BigInt)
  _GeneratorFunction = JSGeneratorFunctionConstructor::initialize(this);
  CHECK(this, _GeneratorFunction);
  _Generator = JSGeneratorConstructor::initialize(this);
  CHECK(this, _Generator);
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

JSValue *JSContext::isUninitialized(JSValue *value) {
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

JSValue *JSContext::createInfinity(bool negative) {
  return _current->createValue(
      _runtime->getAllocator()->create<JSInfinity>(negative));
}

JSValue *JSContext::createBigInt(const BigInt<> &bigint) {
  return _current->createValue(
      _runtime->getAllocator()->create<JSBigInt>(bigint));
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

  auto object = _current->createValue(getAllocator()->create<JSObject>());
  auto obj = object->getData()->cast<JSObject>();
  if (_Object) {
    if (!prototype) {
      prototype = getField(_Object, createString(L"prototype"));
      auto err = setConstructor(object, _Object);
      CHECK(this, err);
    }
  }
  if (!prototype) {
    prototype = createNull();
  }
  CHECK(this, prototype);
  obj->setPrototype(prototype->getAtom());
  object->getAtom()->addChild(prototype->getAtom());
  return object;
}

JSValue *JSContext::createArray() { return construct(_Array); }

JSValue *JSContext::createInterrupt(const JSEvalContext &ectx, JSValue *value) {
  auto interrupt =
      _current->createValue(_runtime->getAllocator()->create<JSInterrupt>(
          ectx, _current, value->getAtom()));
  interrupt->getAtom()->addChild(value->getAtom());
  return interrupt;
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
  auto prototype = createObject();
  auto err = defineProperty(val, createString(L"prototype"), prototype, true,
                            false, true);
  CHECK(this, err);
  err = defineProperty(prototype, createString(L"constructor"), val, true,
                       false, true);
  CHECK(this, err);
  err = defineProperty(val, createString(L"name"), createString(name), false,
                       false, false);
  CHECK(this, err);
  if (_Function) {
    auto prototype = getField(_Function, createString(L"prototype"));
    val->getData()->cast<JSCallable>()->setPrototype(prototype->getAtom());
    val->getAtom()->addChild(prototype->getAtom());
    err = defineProperty(val, createString(L"constructor"), _Function, true,
                         false, true);
    CHECK(this, err);
    err = setConstructor(val, _Function);
    CHECK(this, err);
  } else {
    auto prototype = createObject();
    val->getData()->cast<JSCallable>()->setPrototype(prototype->getAtom());
    val->getAtom()->addChild(prototype->getAtom());
  }
  CHECK(this, err);
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
  auto prototype = createObject();
  auto err = defineProperty(val, createString(L"prototype"), prototype, true,
                            false, true);
  CHECK(this, err);
  err = defineProperty(prototype, createString(L"constructor"), val, true,
                       false, true);
  CHECK(this, err);
  err = defineProperty(val, createString(L"name"), createString(name), false,
                       false, false);
  CHECK(this, err);
  if (_Function) {
    auto prototype = getField(_Function, createString(L"prototype"));
    val->getData()->cast<JSCallable>()->setPrototype(prototype->getAtom());
    val->getAtom()->addChild(prototype->getAtom());
    err = defineProperty(val, createString(L"constructor"), _Function, true,
                         false, true);
    CHECK(this, err);
    err = setConstructor(val, _Function);
    CHECK(this, err);
  } else {
    auto prototype = createObject();
    val->getData()->cast<JSCallable>()->setPrototype(prototype->getAtom());
    val->getAtom()->addChild(prototype->getAtom());
  }
  CHECK(this, err);
  return val;
}

JSValue *JSContext::createGeneratorFunction(
    const std::wstring &name, const std::wstring &filename, size_t address,
    const std::unordered_map<std::wstring, JSValue *> &closure) {
  std::unordered_map<std::wstring, JSAtom *> clo;
  for (auto &[n, val] : closure) {
    clo[n] = val->getAtom();
  }
  auto val = _current->createValue(
      _runtime->getAllocator()->create<JSGeneratorFunction>(name, filename,
                                                            address, clo));
  for (auto &[n, atom] : clo) {
    val->getAtom()->addChild(atom);
  }
  auto prototype = createObject();
  auto err = defineProperty(val, createString(L"prototype"), prototype, true,
                            false, true);
  CHECK(this, err);
  err = defineProperty(prototype, createString(L"constructor"), val, true,
                       false, true);
  CHECK(this, err);
  err = defineProperty(val, createString(L"name"), createString(name), false,
                       false, false);
  CHECK(this, err);
  prototype = getField(_GeneratorFunction, createString(L"prototype"));
  val->getData()->cast<JSCallable>()->setPrototype(prototype->getAtom());
  val->getAtom()->addChild(prototype->getAtom());
  err = defineProperty(val, createString(L"constructor"), _GeneratorFunction,
                       true, false, true);
  CHECK(this, err);
  err = setConstructor(val, _GeneratorFunction);
  CHECK(this, err);
  return val;
}

JSValue *JSContext::createException(const JSException::TYPE &type,
                                    const std::wstring &message,
                                    const std::wstring &filename, size_t column,
                                    size_t line) {
  return _current->createValue(_runtime->getAllocator()->create<JSException>(
      type, message, trace(filename, column, line)));
}

JSValue *JSContext::createException(JSValue *error) {
  auto e = _current->createValue(
      _runtime->getAllocator()->create<JSException>(error->getAtom()));
  e->getAtom()->addChild(error->getAtom());
  return e;
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
    if (atomKey->isTypeof<JSStringType>() &&
        atomKey->getData()->cast<JSString>()->getValue() == name) {
      return getField(_global, createString(name));
    }
  }
  return createException(JSException::TYPE::SYNTAX,
                         std::format(L"'{}' is not defined", name));
}

JSValue *JSContext::assigmentValue(JSValue *variable, JSValue *value) {
  if (variable->isConst() && !variable->isTypeof<JSUninitializeType>()) {
    return createException(JSException::TYPE::TYPE,
                           L"Assignment to constant variable.");
  }
  CHECK(this, value);
  variable->getAtom()->setData(clone(value)->getData());
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

JSValue *JSContext::construct(JSValue *constructor,
                              const std::vector<JSValue *> &args) {
  CHECK(this, constructor);
  auto prototype = getField(constructor, createString(L"prototype"));
  CHECK(this, prototype);
  JSValue *obj = nullptr;
  if (_Array->getData() == constructor->getData()) {
    obj =
        _current->createValue(getRuntime()->getAllocator()->create<JSArray>());
  }
  if (_Generator->getData() == constructor->getData()) {
    obj = _current->createValue(
        getRuntime()->getAllocator()->create<JSGenerator>());
  }
  if (!obj) {
    obj = createObject();
  }
  CHECK(this, obj);
  auto err = setPrototype(obj, prototype);
  CHECK(this, err);
  err = setField(obj, createString(L"constructor"), constructor);
  CHECK(this, err);
  err = setConstructor(obj, constructor);
  CHECK(this, err);
  auto res = call(constructor, obj, args);
  if (res->isTypeof<JSObjectType>()) {
    return res;
  }
  return obj;
}

JSValue *JSContext::getPrototypeOf(JSValue *value) {
  CHECK(this, value);
  if (!value->isTypeof<JSObjectType>()) {
    value = value->getType()->pack(this, value);
  }
  return value->getType()->cast<JSObjectType>()->getPrototypeOf(this, value);
}
JSValue *JSContext::setPrototype(JSValue *value, JSValue *prototype) {
  CHECK(this, value);
  CHECK(this, prototype);
  if (!value->isTypeof<JSObjectType>()) {
    return createException(JSException::TYPE::TYPE,
                           L"variable is not a object");
  }
  return value->getType()->cast<JSObjectType>()->setPrototype(this, value,
                                                              prototype);
}

JSValue *JSContext::getConstructorOf(JSValue *value) {
  CHECK(this, value);
  if (!value->isTypeof<JSObjectType>()) {
    value = value->getType()->pack(this, value);
  }
  return value->getType()->cast<JSObjectType>()->getConstructorOf(this, value);
}

JSValue *JSContext::setConstructor(JSValue *value, JSValue *constructor) {
  CHECK(this, value);
  CHECK(this, constructor);
  if (!value->isTypeof<JSObjectType>()) {
    return createException(JSException::TYPE::TYPE,
                           L"variable is not a object");
  }
  return value->getType()->cast<JSObjectType>()->setConstructor(this, value,
                                                                constructor);
}

JSValue *JSContext::setField(JSValue *obj, JSValue *name, JSValue *value) {
  CHECK(this, obj);
  CHECK(this, name);
  CHECK(this, value);
  auto otype = obj->getType()->cast<JSObjectType>();
  if (!otype) {
    return createException(JSException::TYPE::TYPE,
                           L"variable is not a object");
  }
  return otype->setField(this, obj, name, value);
}

JSValue *JSContext::defineProperty(JSValue *obj, JSValue *name, JSValue *value,
                                   bool configurable, bool enumable,
                                   bool writable) {
  CHECK(this, obj);
  CHECK(this, value);
  auto otype = obj->getType()->cast<JSObjectType>();
  if (!otype) {
    return createException(JSException::TYPE::TYPE,
                           L"variable is not a object");
  }
  return otype->defineProperty(this, obj, name, value, configurable, enumable,
                               writable);
}

JSValue *JSContext::defineProperty(JSValue *obj, JSValue *name, JSValue *getter,
                                   JSValue *setter, bool configurable,
                                   bool enumable) {
  CHECK(this, obj);
  CHECK(this, getter);
  CHECK(this, setter);
  auto otype = obj->getType()->cast<JSObjectType>();
  if (!otype) {
    return createException(JSException::TYPE::TYPE,
                           L"variable is not a object");
  }
  return otype->defineProperty(this, obj, name, getter, setter, configurable,
                               enumable);
}
JSValue *JSContext::getPrivateField(JSValue *obj, const std::wstring &name) {
  CHECK(this, obj);
  auto otype = obj->getType()->cast<JSObjectType>();
  if (!otype) {
    return createException(JSException::TYPE::TYPE,
                           L"variable is not a object");
  }
  return otype->getPrivateField(this, obj, name);
}

JSValue *JSContext::setPrivateField(JSValue *obj, const std::wstring &name,
                                    JSValue *value) {
  CHECK(this, obj);
  CHECK(this, value);
  auto otype = obj->getType()->cast<JSObjectType>();
  if (!otype) {
    return createException(JSException::TYPE::TYPE,
                           L"variable is not a object");
  }
  return otype->setPrivateField(this, obj, name, value);
}

JSValue *JSContext::definePrivateProperty(JSValue *obj,
                                          const std::wstring &name,
                                          JSValue *value) {
  CHECK(this, obj);
  CHECK(this, value);
  auto otype = obj->getType()->cast<JSObjectType>();
  if (!otype) {
    return createException(JSException::TYPE::TYPE,
                           L"variable is not a object");
  }
  return otype->definePrivateProperty(this, obj, name, value);
}

JSValue *JSContext::definePrivateProperty(JSValue *obj,
                                          const std::wstring &name,
                                          JSValue *getter, JSValue *setter) {
  CHECK(this, obj);
  CHECK(this, getter);
  CHECK(this, setter);
  auto otype = obj->getType()->cast<JSObjectType>();
  if (!otype) {
    return createException(JSException::TYPE::TYPE,
                           L"variable is not a object");
  }
  return otype->definePrivateProperty(this, obj, name, getter, setter);
}

JSValue *JSContext::getField(JSValue *obj, JSValue *name) {
  CHECK(this, obj);
  auto otype = obj->getType()->cast<JSObjectType>();
  if (!otype) {
    return createException(JSException::TYPE::TYPE,
                           L"variable is not a object");
  }
  return otype->getField(this, obj, name);
}

JSValue *JSContext::getKeys(JSValue *obj) {
  CHECK(this, obj);
  auto otype = obj->getType()->cast<JSObjectType>();
  if (!otype) {
    return createException(JSException::TYPE::TYPE,
                           L"variable is not a object");
  }
  return otype->getKeys(this, obj);
}

JSValue *JSContext::toString(JSValue *value) {
  return value->getType()->toString(this, value);
}

JSValue *JSContext::toNumber(JSValue *value) {
  CHECK(this, value);
  return value->getType()->toNumber(this, value);
}

JSValue *JSContext::toBoolean(JSValue *value) {
  CHECK(this, value);
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
JSValue *JSContext::unaryNegation(JSValue *value) {
  value = unpack(value);
  if (value->isTypeof<JSBigIntType>()) {
    return value->getType()->cast<JSBigIntType>()->unaryNegation(this, value);
  }
  value = toNumber(value);
  CHECK(this, value);
  return value->getType()->cast<JSNumberType>()->unaryNegation(this, value);
}

JSValue *JSContext::unaryPlus(JSValue *value) {
  value = unpack(value);
  if (value->isTypeof<JSBigIntType>()) {
    return createException(JSException::TYPE::TYPE,
                           L"Cannot convert a BigInt value to a number");
  }
  value = toNumber(value);
  CHECK(this, value);
  return value->getType()->cast<JSNumberType>()->unaryPlus(this, value);
}

JSValue *JSContext::isEqual(JSValue *left, JSValue *right) {
  CHECK(this, left);
  CHECK(this, right);
  if (left->getType() != right->getType()) {
    if ((left->isTypeof<JSNullType>() || left->isTypeof<JSUndefinedType>()) &&
        (right->isTypeof<JSNullType>() || right->isTypeof<JSUndefinedType>())) {
      return createBoolean(true);
    }
    if (left->isTypeof<JSObjectType>() && !right->isTypeof<JSObjectType>()) {
      left = unpack(left);
    }
    if (right->isTypeof<JSObjectType>() && !left->isTypeof<JSObjectType>()) {
      right = unpack(right);
    }
  }
  if (left->getType() != right->getType()) {
    if (right->isTypeof<JSSymbolType>()) {
      auto tmp = right;
      right = left;
      left = tmp;
    }
    if (!left->isTypeof<JSObjectType>() && !right->isTypeof<JSObjectType>() &&
        !left->isTypeof<JSSymbolType>()) {
      if (left->isTypeof<JSBooleanType>() || right->isTypeof<JSBooleanType>()) {
        left = toNumber(left);
        right = toNumber(right);
      }
      if (right->isTypeof<JSBigIntType>()) {
        auto tmp = left;
        left = right;
        right = tmp;
      }
      if (left->isTypeof<JSBigIntType>()) {
        if (right->isTypeof<JSStringType>()) {
          CHECK(this, right);
          right = createBigInt(checkedString(right));
        } else {
          auto num = toNumber(right);
          CHECK(this, num);
          right = createBigInt(checkedNumber(num));
        }
      }
      if (right->isTypeof<JSNumberType>()) {
        auto tmp = left;
        left = right;
        right = tmp;
      }
      if (left->isTypeof<JSNumberType>()) {
        if (right->isTypeof<JSStringType>()) {
          right = toNumber(right);
        }
      }
      if (right->isTypeof<JSStringType>()) {
        auto tmp = left;
        left = right;
        right = tmp;
      }
      if (left->isTypeof<JSStringType>()) {
        right = toString(right);
      }
    }
  }
  if (left->isTypeof<JSObjectType>() && !right->isTypeof<JSObjectType>()) {
    return createBoolean(false);
  }
  if (right->isTypeof<JSObjectType>() && !left->isTypeof<JSObjectType>()) {
    return createBoolean(false);
  }
  CHECK(this, left);
  CHECK(this, right);
  return left->getType()->equal(this, left, right);
}

JSValue *JSContext::add(JSValue *left, JSValue *right) {
  left = unpack(left);
  right = unpack(right);
  if (left->isTypeof<JSBigIntType>() && !right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (!left->isTypeof<JSBigIntType>() && right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (left->isTypeof<JSBigIntType>()) {
    return left->getType()->cast<JSBigIntType>()->add(this, left, right);
  }
  if (left->isTypeof<JSStringType>() || right->isTypeof<JSStringType>()) {
    left = toString(left);
    right = toString(right);
    return left->getType()->cast<JSStringType>()->add(this, left, right);
  } else {
    left = toNumber(left);
    right = toNumber(right);
    return left->getType()->cast<JSNumberType>()->add(this, left, right);
  }
}

JSValue *JSContext::sub(JSValue *left, JSValue *right) {
  left = unpack(left);
  right = unpack(right);
  if (left->isTypeof<JSBigIntType>() && !right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (!left->isTypeof<JSBigIntType>() && right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (left->isTypeof<JSBigIntType>()) {
    return left->getType()->cast<JSBigIntType>()->sub(this, left, right);
  }
  left = toNumber(left);
  right = toNumber(right);
  CHECK(this, left);
  CHECK(this, right);
  return left->getType()->cast<JSNumberType>()->sub(this, left, right);
}

JSValue *JSContext::mul(JSValue *left, JSValue *right) {
  left = unpack(left);
  right = unpack(right);
  if (left->isTypeof<JSBigIntType>() && !right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (!left->isTypeof<JSBigIntType>() && right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (left->isTypeof<JSBigIntType>()) {
    return left->getType()->cast<JSBigIntType>()->mul(this, left, right);
  }
  left = toNumber(left);
  right = toNumber(right);
  CHECK(this, left);
  CHECK(this, right);
  return left->getType()->cast<JSNumberType>()->mul(this, left, right);
}

JSValue *JSContext::div(JSValue *left, JSValue *right) {
  left = unpack(left);
  right = unpack(right);
  if (left->isTypeof<JSBigIntType>() && !right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (!left->isTypeof<JSBigIntType>() && right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (left->isTypeof<JSBigIntType>()) {
    return left->getType()->cast<JSBigIntType>()->div(this, left, right);
  }
  left = toNumber(left);
  right = toNumber(right);
  CHECK(this, left);
  CHECK(this, right);
  return left->getType()->cast<JSNumberType>()->div(this, left, right);
}

JSValue *JSContext::mod(JSValue *left, JSValue *right) {
  left = unpack(left);
  right = unpack(right);
  if (left->isTypeof<JSBigIntType>() && !right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (!left->isTypeof<JSBigIntType>() && right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (left->isTypeof<JSBigIntType>()) {
    return left->getType()->cast<JSBigIntType>()->mod(this, left, right);
  }
  left = toNumber(left);
  right = toNumber(right);
  CHECK(this, left);
  CHECK(this, right);
  return left->getType()->cast<JSNumberType>()->mod(this, left, right);
}

JSValue *JSContext::pow(JSValue *left, JSValue *right) {
  left = unpack(left);
  right = unpack(right);
  if (left->isTypeof<JSBigIntType>() && !right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (!left->isTypeof<JSBigIntType>() && right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (left->isTypeof<JSBigIntType>()) {
    return left->getType()->cast<JSBigIntType>()->pow(this, left, right);
  }
  left = toNumber(left);
  right = toNumber(right);
  CHECK(this, left);
  CHECK(this, right);
  return left->getType()->cast<JSNumberType>()->pow(this, left, right);
}

JSValue *JSContext::and_(JSValue *left, JSValue *right) {
  left = unpack(left);
  right = unpack(right);
  if (left->isTypeof<JSBigIntType>() && !right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (!left->isTypeof<JSBigIntType>() && right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (left->isTypeof<JSBigIntType>()) {
    return left->getType()->cast<JSBigIntType>()->and_(this, left, right);
  }
  left = toNumber(left);
  right = toNumber(right);
  CHECK(this, left);
  CHECK(this, right);
  return left->getType()->cast<JSNumberType>()->and_(this, left, right);
}

JSValue *JSContext::or_(JSValue *left, JSValue *right) {
  left = unpack(left);
  right = unpack(right);
  if (left->isTypeof<JSBigIntType>() && !right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (!left->isTypeof<JSBigIntType>() && right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (left->isTypeof<JSBigIntType>()) {
    return left->getType()->cast<JSBigIntType>()->or_(this, left, right);
  }
  left = toNumber(left);
  right = toNumber(right);
  CHECK(this, left);
  CHECK(this, right);
  return left->getType()->cast<JSNumberType>()->or_(this, left, right);
}

JSValue *JSContext::xor_(JSValue *left, JSValue *right) {
  left = unpack(left);
  right = unpack(right);
  if (left->isTypeof<JSBigIntType>() && !right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (!left->isTypeof<JSBigIntType>() && right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (left->isTypeof<JSBigIntType>()) {
    return left->getType()->cast<JSBigIntType>()->xor_(this, left, right);
  }
  left = toNumber(left);
  right = toNumber(right);
  CHECK(this, left);
  CHECK(this, right);
  return left->getType()->cast<JSNumberType>()->xor_(this, left, right);
}

JSValue *JSContext::shr(JSValue *left, JSValue *right) {
  left = unpack(left);
  right = unpack(right);
  if (left->isTypeof<JSBigIntType>() && !right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (!left->isTypeof<JSBigIntType>() && right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (left->isTypeof<JSBigIntType>()) {
    return left->getType()->cast<JSBigIntType>()->shr(this, left, right);
  }
  left = toNumber(left);
  right = toNumber(right);
  CHECK(this, left);
  CHECK(this, right);
  return left->getType()->cast<JSNumberType>()->shr(this, left, right);
}

JSValue *JSContext::shl(JSValue *left, JSValue *right) {
  left = unpack(left);
  right = unpack(right);
  if (left->isTypeof<JSBigIntType>() && !right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (!left->isTypeof<JSBigIntType>() && right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (left->isTypeof<JSBigIntType>()) {
    return left->getType()->cast<JSBigIntType>()->shl(this, left, right);
  }
  left = toNumber(left);
  right = toNumber(right);
  CHECK(this, left);
  CHECK(this, right);
  return left->getType()->cast<JSNumberType>()->shl(this, left, right);
}

JSValue *JSContext::gt(JSValue *left, JSValue *right) {
  left = unpack(left);
  right = unpack(right);
  if (left->isTypeof<JSBigIntType>() && !right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (!left->isTypeof<JSBigIntType>() && right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (left->isTypeof<JSBigIntType>()) {
    return left->getType()->cast<JSBigIntType>()->gt(this, left, right);
  }
  left = toNumber(left);
  right = toNumber(right);
  CHECK(this, left);
  CHECK(this, right);
  return left->getType()->cast<JSNumberType>()->gt(this, left, right);
}

JSValue *JSContext::ge(JSValue *left, JSValue *right) {
  left = unpack(left);
  right = unpack(right);
  if (left->isTypeof<JSBigIntType>() && !right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (!left->isTypeof<JSBigIntType>() && right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (left->isTypeof<JSBigIntType>()) {
    return left->getType()->cast<JSBigIntType>()->ge(this, left, right);
  }
  left = toNumber(left);
  right = toNumber(right);
  CHECK(this, left);
  CHECK(this, right);
  return left->getType()->cast<JSNumberType>()->ge(this, left, right);
}

JSValue *JSContext::lt(JSValue *left, JSValue *right) {
  left = unpack(left);
  right = unpack(right);
  if (left->isTypeof<JSBigIntType>() && !right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (!left->isTypeof<JSBigIntType>() && right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (left->isTypeof<JSBigIntType>()) {
    return left->getType()->cast<JSBigIntType>()->lt(this, left, right);
  }
  left = toNumber(left);
  right = toNumber(right);
  CHECK(this, left);
  CHECK(this, right);
  return left->getType()->cast<JSNumberType>()->lt(this, left, right);
}

JSValue *JSContext::le(JSValue *left, JSValue *right) {
  left = unpack(left);
  right = unpack(right);
  if (left->isTypeof<JSBigIntType>() && !right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (!left->isTypeof<JSBigIntType>() && right->isTypeof<JSBigIntType>()) {
    return createException(
        JSException::TYPE::TYPE,
        L"Cannot mix BigInt and other types, use explicit conversions");
  }
  if (left->isTypeof<JSBigIntType>()) {
    return left->getType()->cast<JSBigIntType>()->le(this, left, right);
  }
  left = toNumber(left);
  right = toNumber(right);
  CHECK(this, left);
  CHECK(this, right);
  return left->getType()->cast<JSNumberType>()->le(this, left, right);
}

JSValue *JSContext::not_(JSValue *value) {
  value = unpack(value);
  if (value->isTypeof<JSBigIntType>()) {
    return value->getType()->cast<JSBigIntType>()->not_(this, value);
  }
  value = toNumber(value);
  CHECK(this, value);
  return value->getType()->cast<JSNumberType>()->not_(this, value);
}
JSValue *JSContext::inc(JSValue *value) {
  value = unpack(value);
  if (value->isTypeof<JSBigIntType>()) {
    return value->getType()->cast<JSBigIntType>()->inc(this, value);
  }
  value = toNumber(value);
  CHECK(this, value);
  return value->getType()->cast<JSNumberType>()->inc(this, value);
}

JSValue *JSContext::dec(JSValue *value) {
  value = unpack(value);
  if (value->isTypeof<JSBigIntType>()) {
    return value->getType()->cast<JSBigIntType>()->dec(this, value);
  }
  value = toNumber(value);
  CHECK(this, value);
  return value->getType()->cast<JSNumberType>()->dec(this, value);
}

void JSContext::setMetadata(JSValue *value, const std::wstring &name,
                            JSValue *metadata) {
  auto base = value->getData();
  auto &meta = base->getMetadata();
  if (meta.contains(name)) {
    value->getAtom()->removeChild(meta.at(name));
  }
  if (metadata) {
    value->getAtom()->addChild(metadata->getAtom());
    meta[name] = metadata->getAtom();
  } else {
    meta.erase(name);
  }
}

JSValue *JSContext::getMetadata(JSValue *value, const std::wstring &name) {
  auto base = value->getData();
  auto &meta = base->getMetadata();
  if (meta.contains(name)) {
    return createValue(meta.at(name));
  }
  return nullptr;
}

JSValue *JSContext::pack(JSValue *value) {
  CHECK(this, value);
  return value->getType()->pack(this, value);
}
JSValue *JSContext::unpack(JSValue *value) {
  if (value->isTypeof<JSObjectType>()) {
    auto toPrimitiveSymbol = getField(_Symbol, createString(L"toPrimitive"));
    CHECK(this, toPrimitiveSymbol);
    auto toPrimitive = getField(value, toPrimitiveSymbol);
    CHECK(this, toPrimitive);
    if (toPrimitive->isTypeof<JSCallableType>()) {
      value = call(toPrimitive, value, {createString(L"default")});
      CHECK(this, value);
      if (value->isTypeof<JSObjectType>()) {
        return createException(JSException::TYPE::TYPE,
                               L"Cannot convert object to primitive value");
      }
    }
    if (!value->isTypeof<JSObjectType>()) {
      return value;
    }
    auto valueOf = getField(value, createString(L"valueOf"));
    CHECK(this, valueOf);
    if (valueOf->isTypeof<JSCallableType>()) {
      auto res = call(valueOf, value, {});
      CHECK(this, res);
      if (!res->isTypeof<JSObjectType>()) {
        value = res;
      }
    }
    if (!value->isTypeof<JSObjectType>()) {
      return value;
    }
    value = toString(value);
  }
  return value;
}