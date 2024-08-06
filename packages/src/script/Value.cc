#include "script/Value.hpp"
#include "core/AutoPtr.hpp"
#include "script/Atom.hpp"
#include "script/Record.hpp"
#include <any>
using namespace firefly;
using namespace firefly::script;
Value::Value(Atom *atom) : _atom(atom) {}
Atom::Type Value::getType() { return _atom->_type; }
double Value::toNumber() {
  if (getType() == Atom::Type::NUMBER) {
    return std::any_cast<double>(_atom->_value);
  }
  return 0;
}
int64_t Value::toInteger() {
  if (getType() == Atom::Type::INTEGER) {
    return std::any_cast<int64_t>(_atom->_value);
  }
  return 0;
}
bool Value::toBoolean() {
  if (getType() == Atom::Type::BOOLEAN) {
    return std::any_cast<bool>(_atom->_value);
  }
  return false;
}
std::string Value::toString() {
  if (getType() == Atom::Type::STRING) {
    return std::any_cast<std::string>(_atom->_value);
  }
  return "";
}
core::AutoPtr<Value> Value::setNumber(const double &value) {
  _atom->_type = Atom::Type::NUMBER;
  _atom->_value = double(value);
  return this;
}
core::AutoPtr<Value> Value::setInteger(const int64_t &value) {
  _atom->_type = Atom::Type::INTEGER;
  _atom->_value = int64_t(value);
  return this;
}
core::AutoPtr<Value> Value::setBoolean(const bool &value) {
  _atom->_type = Atom::Type::BOOLEAN;
  _atom->_value = bool(value);
  return this;
}
core::AutoPtr<Value> Value::setString(const std::string &value) {
  _atom->_type = Atom::Type::STRING;
  _atom->_value = std::string(value);
  return this;
}
core::AutoPtr<Value> Value::setNil() {
  _atom->_type = Atom::Type::NIL;
  _atom->_value = nullptr;
  return this;
}
core::AutoPtr<Value> Value::setObject() {
  _atom->_type = Atom::Type::OBJECT;
  _atom->_value = core::AutoPtr(new Record(_atom));
  return this;
}
core::AutoPtr<Value> Value::setArray() {
  _atom->_type = Atom::Type::ARRAY;
  return this;
}
core::AutoPtr<Value> Value::setFunction(const FunctionHandle &func) {
  _atom->_type = Atom::Type::FUNCTION;
  _atom->_value = FunctionHandle(func);
  return this;
}

Value::Stack Value::call(core::AutoPtr<Context> ctx, Value::Stack args) {
  auto func = std::any_cast<FunctionHandle>(_atom->_value);
  auto current = ctx->pushScope();
  Value::Stack result;
  for (auto &arg : args) {
    auto dep = ctx->getCurrentScope()->getRoot();
    arg->getAtom()->_parent.push_back(dep);
    dep->_children.push_back(arg->getAtom());
  }
  result = func(ctx, args);
  for (auto &item : result) {
    auto dep = current->getRoot();
    item->getAtom()->_parent.push_back(dep);
    dep->_children.push_back(item->getAtom());
  }
  ctx->popScope(current);
  return result;
}

core::AutoPtr<Value> Value::setField(core::AutoPtr<Context> ctx,
                                     const std::string &name,
                                     core::AutoPtr<Value> field) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto setter = metadata->getField(ctx, "set");
    if (setter->getType() != Atom::Type::NIL) {
      setter->call(ctx, {this, ctx->createValue()->setString(name), field});
      return this;
    }
  }
  std::any_cast<core::AutoPtr<Record>>(_atom->_value)
      ->setField(ctx, name, field);
  return this;
}
core::AutoPtr<Value> Value::getField(core::AutoPtr<Context> ctx,
                                     const std::string &name) {

  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto getter = metadata->getField(ctx, "get");
    if (getter->getType() != Atom::Type::NIL) {
      return getter->call(ctx, {this, ctx->createValue()->setString(name)})[0];
    }
  }
  auto res =
      std::any_cast<core::AutoPtr<Record>>(_atom->_value)->getField(ctx, name);
  return res;
}
core::AutoPtr<Value> Value::setIndex(core::AutoPtr<Context> ctx,
                                     const uint32_t &name,
                                     core::AutoPtr<Value> field) {
  return this;
}
core::AutoPtr<Value> Value::getIndex(core::AutoPtr<Context> ctx,
                                     const uint32_t &name) {
  return nullptr;
}
uint32_t Value::getLength(core::AutoPtr<Context> ctx) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto length = metadata->getField(ctx, "length");
    if (length->getType() != Atom::Type::NIL) {
      return length->call(ctx, {this})[0]->toInteger();
    }
  }
  if (getType() == Atom::Type::OBJECT) {
    return std::any_cast<core::AutoPtr<Record>>(_atom->_value)->getLength(ctx);
  }
  return 0;
}
core::AutoPtr<Value> Value::getMetadata(core::AutoPtr<Context> ctx) {
  return ctx->createValue(_atom->_metadata);
}
core::AutoPtr<Value> Value::setMetadata(core::AutoPtr<Value> value) {
  _atom->_metadata = value->getAtom();
  return this;
}

core::AutoPtr<Value> Value::setRawField(core::AutoPtr<Context> ctx,
                                        const std::string &name,
                                        core::AutoPtr<Value> field) {
  std::any_cast<core::AutoPtr<Record>>(_atom->_value)
      ->setField(ctx, name, field);
  return this;
}
core::AutoPtr<Value> Value::getRawField(core::AutoPtr<Context> ctx,
                                        const std::string &name) {
  return std::any_cast<core::AutoPtr<Record>>(_atom->_value)
      ->getField(ctx, name);
}
core::AutoPtr<Value> Value::setRawIndex(core::AutoPtr<Context> ctx,
                                        const uint32_t &name,
                                        core::AutoPtr<Value> field) {
  return this;
}
core::AutoPtr<Value> Value::getRawIndex(core::AutoPtr<Context> ctx,
                                        const uint32_t &name) {
  return nullptr;
}
uint32_t Value::getRawLength(core::AutoPtr<Context> ctx) {
  return std::any_cast<core::AutoPtr<Record>>(_atom->_value)->getLength(ctx);
}

core::AutoPtr<Value> Value::arithmetic(core::AutoPtr<Value> another,
                                       int operation) {
  return nullptr;
}
Atom *Value::getAtom() { return _atom; }