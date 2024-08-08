#include "script/Value.hpp"
#include "core/AutoPtr.hpp"
#include "script/Array.hpp"
#include "script/Atom.hpp"
#include "script/Record.hpp"
#include <any>
#include <fmt/core.h>
#include <stdexcept>
#include <unordered_map>
#include <vector>
using namespace firefly;
using namespace firefly::script;
Value::Value(Atom *atom) : _atom(atom) {}
Atom::Type Value::getType(core::AutoPtr<Context> ctx) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto valueOf = metadata->getField(ctx, "typeof");
    if (valueOf->getType(ctx) != Atom::Type::NIL) {
      auto value = valueOf->call(ctx, {this})[0];
      return (Atom::Type)value->toNumber(ctx);
    }
  }
  return _atom->_type;
}

std::string Value::getTypeName(core::AutoPtr<Context> ctx) {
  switch (getType(ctx)) {
  case Atom::Type::NIL:
    return "Nil";
  case Atom::Type::NUMBER:
    return "Number";
  case Atom::Type::BOOLEAN:
    return "Boolean";
  case Atom::Type::STRING:
    return "String";
  case Atom::Type::OBJECT:
    return "Object";
  case Atom::Type::ARRAY:
    return "Array";
  case Atom::Type::FUNCTION:
    return "Function";
  }
  return "Unknown";
}
double Value::toNumber(core::AutoPtr<Context> ctx) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto valueOf = metadata->getField(ctx, "valueOf");
    if (valueOf->getType(ctx) != Atom::Type::NIL) {
      auto value = valueOf->call(ctx, {this})[0];
      if (value != this) {
        return value->toNumber(ctx);
      }
    }
  }
  if (getType(ctx) == Atom::Type::NUMBER) {
    return std::any_cast<double>(_atom->_value);
  }
  return 0;
}
bool Value::toBoolean(core::AutoPtr<Context> ctx) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto valueOf = metadata->getField(ctx, "valueOf");
    if (valueOf->getType(ctx) != Atom::Type::NIL) {
      auto value = valueOf->call(ctx, {this})[0];
      if (value != this) {
        return value->toBoolean(ctx);
      }
    }
  }
  if (getType(ctx) == Atom::Type::BOOLEAN) {
    return std::any_cast<bool>(_atom->_value);
  }
  return false;
}
std::string Value::toString(core::AutoPtr<Context> ctx) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto valueOf = metadata->getField(ctx, "valueOf");
    if (valueOf->getType(ctx) != Atom::Type::NIL) {
      auto value = valueOf->call(ctx, {this})[0];
      if (value != this) {
        return value->toString(ctx);
      }
    }
  }
  if (getType(ctx) == Atom::Type::STRING) {
    return std::any_cast<std::string>(_atom->_value);
  }
  return "";
}
core::AutoPtr<Value> Value::setNumber(core::AutoPtr<Context> ctx,
                                      const double &value) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto ofValue = metadata->getField(ctx, "ofValue");
    if (ofValue->getType(ctx) != Atom::Type::NIL) {
      return ofValue->call(
          ctx, {this, ctx->createValue()->setNumber(ctx, value)})[0];
    }
  }
  _atom->_type = Atom::Type::NUMBER;
  _atom->_value = double(value);
  return this;
}
core::AutoPtr<Value> Value::setBoolean(core::AutoPtr<Context> ctx,
                                       const bool &value) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto ofValue = metadata->getField(ctx, "ofValue");
    if (ofValue->getType(ctx) != Atom::Type::NIL) {
      return ofValue->call(
          ctx, {this, ctx->createValue()->setBoolean(ctx, value)})[0];
    }
  }
  _atom->_type = Atom::Type::BOOLEAN;
  _atom->_value = bool(value);
  return this;
}
core::AutoPtr<Value> Value::setString(core::AutoPtr<Context> ctx,
                                      const std::string &value) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto ofValue = metadata->getField(ctx, "ofValue");
    if (ofValue->getType(ctx) != Atom::Type::NIL) {
      return ofValue->call(
          ctx, {this, ctx->createValue()->setString(ctx, value)})[0];
    }
  }
  _atom->_type = Atom::Type::STRING;
  _atom->_value = std::string(value);
  return this;
}
core::AutoPtr<Value> Value::setNil(core::AutoPtr<Context> ctx) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto ofValue = metadata->getField(ctx, "ofValue");
    if (ofValue->getType(ctx) != Atom::Type::NIL) {
      return ofValue->call(ctx, {this, ctx->createValue()})[0];
    }
  }
  _atom->_type = Atom::Type::NIL;
  _atom->_value = nullptr;
  return this;
}
core::AutoPtr<Value> Value::setObject(core::AutoPtr<Context> ctx) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto ofValue = metadata->getField(ctx, "ofValue");
    if (ofValue->getType(ctx) != Atom::Type::NIL) {
      return ofValue->call(ctx, {this, ctx->createValue()->setObject(ctx)})[0];
    }
  }
  _atom->_type = Atom::Type::OBJECT;
  _atom->_value = core::AutoPtr(new Record(_atom));
  return this;
}
core::AutoPtr<Value> Value::setArray(core::AutoPtr<Context> ctx) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto ofValue = metadata->getField(ctx, "ofValue");
    if (ofValue->getType(ctx) != Atom::Type::NIL) {
      return ofValue->call(ctx, {this, ctx->createValue()->setArray(ctx)})[0];
    }
  }
  _atom->_type = Atom::Type::ARRAY;
  _atom->_value = core::AutoPtr(new Array(_atom));
  return this;
}
core::AutoPtr<Value> Value::setFunction(core::AutoPtr<Context> ctx,
                                        const FunctionHandle &func) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto ofValue = metadata->getField(ctx, "ofValue");
    if (ofValue->getType(ctx) != Atom::Type::NIL) {
      return ofValue->call(
          ctx, {this, ctx->createValue()->setFunction(ctx, func)})[0];
    }
  }
  _atom->_type = Atom::Type::FUNCTION;
  _atom->_value = FunctionHandle(func);
  return this;
}
std::vector<std::string> Value::getKeys(core::AutoPtr<Context> ctx) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto keys = metadata->getField(ctx, "keys");
    if (keys->getType(ctx) != Atom::Type::NIL) {
      std::vector<std::string> result;
      auto res = keys->call(ctx, {this})[0];
      for (auto i = 0; i < res->getLength(ctx); i++) {
        result.push_back(res->getIndex(ctx, i)->toString(ctx));
      }
      return result;
    }
  }
  return std::any_cast<core::AutoPtr<Record>>(_atom->_value)->getKeys();
}

Value::Stack Value::call(core::AutoPtr<Context> ctx, Value::Stack args) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto call = metadata->getField(ctx, "call");
    if (call->getType(ctx) != Atom::Type::NIL) {
      Value::Stack stack = {this};
      for (auto &arg : args) {
        stack.push_back(arg);
      }
      return call->call(ctx, stack);
    }
  }
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
    if (setter->getType(ctx) != Atom::Type::NIL) {
      setter->call(ctx,
                   {this, ctx->createValue()->setString(ctx, name), field});
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
    if (getter->getType(ctx) != Atom::Type::NIL) {
      return getter->call(ctx,
                          {this, ctx->createValue()->setString(ctx, name)})[0];
    }
  }
  auto res =
      std::any_cast<core::AutoPtr<Record>>(_atom->_value)->getField(ctx, name);
  return res;
}
core::AutoPtr<Value> Value::setIndex(core::AutoPtr<Context> ctx,
                                     const uint32_t &name,
                                     core::AutoPtr<Value> field) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto setter = metadata->getField(ctx, "set");
    if (setter->getType(ctx) != Atom::Type::NIL) {
      setter->call(ctx,
                   {this, ctx->createValue()->setNumber(ctx, name), field});
      return this;
    }
  }
  std::any_cast<core::AutoPtr<Array>>(_atom->_value)
      ->setIndex(ctx, name, field);
  return this;
}
core::AutoPtr<Value> Value::getIndex(core::AutoPtr<Context> ctx,
                                     const uint32_t &name) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto getter = metadata->getField(ctx, "get");
    if (getter->getType(ctx) != Atom::Type::NIL) {
      return getter->call(ctx,
                          {this, ctx->createValue()->setNumber(ctx, name)})[0];
    }
  }
  auto res =
      std::any_cast<core::AutoPtr<Array>>(_atom->_value)->getIndex(ctx, name);
  return res;
}
uint32_t Value::getLength(core::AutoPtr<Context> ctx) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto length = metadata->getField(ctx, "length");
    if (length->getType(ctx) != Atom::Type::NIL) {
      return length->call(ctx, {this})[0]->toNumber(ctx);
    }
  }
  if (getType(ctx) == Atom::Type::OBJECT) {
    return std::any_cast<core::AutoPtr<Record>>(_atom->_value)->getLength(ctx);
  }
  if (getType(ctx) == Atom::Type::ARRAY) {
    return std::any_cast<core::AutoPtr<Array>>(_atom->_value)->getLength(ctx);
  }
  return 0;
}
core::AutoPtr<Value> Value::getMetadata(core::AutoPtr<Context> ctx) {
  return ctx->createValue(_atom->_metadata);
}
core::AutoPtr<Value> Value::setMetadata(core::AutoPtr<Value> value) {
  auto metadata = value->getAtom();
  std::unordered_map<ptrdiff_t, Atom *> cache;
  while (metadata) {
    if (cache.contains((ptrdiff_t)metadata)) {
      throw std::runtime_error("Failed to set metadata: cycle metadata");
    }
    cache[(ptrdiff_t)metadata] = metadata;
    metadata = metadata->_metadata;
  }
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
  std::any_cast<core::AutoPtr<Array>>(_atom->_value)
      ->setIndex(ctx, name, field);
  return this;
}
core::AutoPtr<Value> Value::getRawIndex(core::AutoPtr<Context> ctx,
                                        const uint32_t &name) {
  auto res =
      std::any_cast<core::AutoPtr<Array>>(_atom->_value)->getIndex(ctx, name);
  return res;
}
uint32_t Value::getRawLength(core::AutoPtr<Context> ctx) {
  return std::any_cast<core::AutoPtr<Record>>(_atom->_value)->getLength(ctx);
}

core::AutoPtr<Value> Value::arithmetic(core::AutoPtr<Context> ctx,
                                       core::AutoPtr<Value> another,
                                       Operation operation) {
  switch (operation) {
  case Operation::CONNECT:
    return optConnect(ctx, another);
  case Operation::ADD:
    return optAdd(ctx, another);
  case Operation::SUB:
    return optSub(ctx, another);
  case Operation::MUL:
    return optMul(ctx, another);
  case Operation::DIV:
    return optDiv(ctx, another);
  case Operation::MOD:
    return optMod(ctx, another);
  case Operation::EQ:
    return optEq(ctx, another);
  case Operation::NE:
    return optNe(ctx, another);
  case Operation::GT:
    return optGt(ctx, another);
  case Operation::GE:
    return optGe(ctx, another);
  case Operation::LT:
    return optLt(ctx, another);
  case Operation::LE:
    return optLe(ctx, another);
  case Operation::AND:
    return optAnd(ctx, another);
  case Operation::OR:
    return optOr(ctx, another);
  case Operation::XOR:
    return optXor(ctx, another);
  case Operation::NOT:
    return optNot(ctx);
  case Operation::SHR:
    return optShr(ctx, another);
  case Operation::SHL:
    return optShl(ctx, another);
  }
  return nullptr;
}

core::AutoPtr<Value> Value::optConnect(core::AutoPtr<Context> ctx,
                                       core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "connect");
    if (opt->getType(ctx) != Atom::Type::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::Type::STRING ||
      another->getType(ctx) != Atom::Type::STRING) {
    throw std::runtime_error(fmt::format(
        "attempt to perform connect between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto source = toString(ctx);
  auto target = another->toString(ctx);
  return ctx->createValue()->setString(ctx, source + target);
}
core::AutoPtr<Value> Value::optAdd(core::AutoPtr<Context> ctx,
                                   core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "add");
    if (opt->getType(ctx) != Atom::Type::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::Type::NUMBER ||
      another->getType(ctx) != Atom::Type::NUMBER) {
    throw std::runtime_error(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, left + right);
}
core::AutoPtr<Value> Value::optSub(core::AutoPtr<Context> ctx,
                                   core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "subtract");
    if (opt->getType(ctx) != Atom::Type::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::Type::NUMBER ||
      another->getType(ctx) != Atom::Type::NUMBER) {
    throw std::runtime_error(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, left - right);
}
core::AutoPtr<Value> Value::optMul(core::AutoPtr<Context> ctx,
                                   core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "times");
    if (opt->getType(ctx) != Atom::Type::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::Type::NUMBER ||
      another->getType(ctx) != Atom::Type::NUMBER) {
    throw std::runtime_error(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, left * right);
}
core::AutoPtr<Value> Value::optDiv(core::AutoPtr<Context> ctx,
                                   core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "over");
    if (opt->getType(ctx) != Atom::Type::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::Type::NUMBER ||
      another->getType(ctx) != Atom::Type::NUMBER) {
    throw std::runtime_error(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, left / right);
}
core::AutoPtr<Value> Value::optMod(core::AutoPtr<Context> ctx,
                                   core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "module");
    if (opt->getType(ctx) != Atom::Type::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::Type::NUMBER ||
      another->getType(ctx) != Atom::Type::NUMBER) {
    throw std::runtime_error(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  auto res = left;
  while (res >= right) {
    res -= right;
  }
  return ctx->createValue()->setNumber(ctx, res);
}
core::AutoPtr<Value> Value::optAnd(core::AutoPtr<Context> ctx,
                                   core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "and");
    if (opt->getType(ctx) != Atom::Type::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::Type::NUMBER ||
      another->getType(ctx) != Atom::Type::NUMBER) {
    throw std::runtime_error(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, (uint32_t)left & (uint32_t)right);
}
core::AutoPtr<Value> Value::optOr(core::AutoPtr<Context> ctx,
                                  core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "or");
    if (opt->getType(ctx) != Atom::Type::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::Type::NUMBER ||
      another->getType(ctx) != Atom::Type::NUMBER) {
    throw std::runtime_error(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, (uint32_t)left | (uint32_t)right);
}
core::AutoPtr<Value> Value::optXor(core::AutoPtr<Context> ctx,
                                   core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "xor");
    if (opt->getType(ctx) != Atom::Type::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::Type::NUMBER ||
      another->getType(ctx) != Atom::Type::NUMBER) {
    throw std::runtime_error(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, (uint32_t)left ^ (uint32_t)right);
}
core::AutoPtr<Value> Value::optEq(core::AutoPtr<Context> ctx,
                                  core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "equal");
    if (opt->getType(ctx) != Atom::Type::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != another->getType(ctx)) {
    return ctx->createValue()->setBoolean(ctx, false);
  }
  switch (getType(ctx)) {
  case Atom::Type::NIL:
    return ctx->createValue()->setBoolean(ctx, true);
  case Atom::Type::NUMBER:
    return ctx->createValue()->setBoolean(ctx, toNumber(ctx) ==
                                                   another->toNumber(ctx));
  case Atom::Type::BOOLEAN:
    return ctx->createValue()->setBoolean(ctx, toBoolean(ctx) ==
                                                   another->toBoolean(ctx));
  case Atom::Type::STRING:
    return ctx->createValue()->setBoolean(ctx, toString(ctx) ==
                                                   another->toString(ctx));
  default:
    return ctx->createValue()->setBoolean(ctx, _atom == another->_atom);
  }
}
core::AutoPtr<Value> Value::optNe(core::AutoPtr<Context> ctx,
                                  core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "notEqual");
    if (opt->getType(ctx) != Atom::Type::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != another->getType(ctx)) {
    return ctx->createValue()->setBoolean(ctx, false);
  }

  switch (getType(ctx)) {
  case Atom::Type::NIL:
    return ctx->createValue()->setBoolean(ctx, false);
  case Atom::Type::NUMBER:
    return ctx->createValue()->setBoolean(ctx, toNumber(ctx) !=
                                                   another->toNumber(ctx));
  case Atom::Type::BOOLEAN:
    return ctx->createValue()->setBoolean(ctx, toBoolean(ctx) !=
                                                   another->toBoolean(ctx));
  case Atom::Type::STRING:
    return ctx->createValue()->setBoolean(ctx, toString(ctx) !=
                                                   another->toString(ctx));
  default:
    return ctx->createValue()->setBoolean(ctx, _atom != another->_atom);
  }
}
core::AutoPtr<Value> Value::optGt(core::AutoPtr<Context> ctx,
                                  core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "greaterThan");
    if (opt->getType(ctx) != Atom::Type::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::Type::NUMBER ||
      another->getType(ctx) != Atom::Type::NUMBER) {
    throw std::runtime_error(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, left > right);
}
core::AutoPtr<Value> Value::optGe(core::AutoPtr<Context> ctx,
                                  core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "greaterAndEqual");
    if (opt->getType(ctx) != Atom::Type::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::Type::NUMBER ||
      another->getType(ctx) != Atom::Type::NUMBER) {
    throw std::runtime_error(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, left >= right);
}
core::AutoPtr<Value> Value::optLt(core::AutoPtr<Context> ctx,
                                  core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "lessThan");
    if (opt->getType(ctx) != Atom::Type::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::Type::NUMBER ||
      another->getType(ctx) != Atom::Type::NUMBER) {
    throw std::runtime_error(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, left < right);
}
core::AutoPtr<Value> Value::optLe(core::AutoPtr<Context> ctx,
                                  core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "lessAndEqual");
    if (opt->getType(ctx) != Atom::Type::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::Type::NUMBER ||
      another->getType(ctx) != Atom::Type::NUMBER) {
    throw std::runtime_error(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, left <= right);
}
core::AutoPtr<Value> Value::optShl(core::AutoPtr<Context> ctx,
                                   core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "leftShifting");
    if (opt->getType(ctx) != Atom::Type::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::Type::NUMBER ||
      another->getType(ctx) != Atom::Type::NUMBER) {
    throw std::runtime_error(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, (uint32_t)left << (uint32_t)right);
}
core::AutoPtr<Value> Value::optShr(core::AutoPtr<Context> ctx,
                                   core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "rightShifting");
    if (opt->getType(ctx) != Atom::Type::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::Type::NUMBER ||
      another->getType(ctx) != Atom::Type::NUMBER) {
    throw std::runtime_error(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, (uint32_t)left >> (uint32_t)right);
}
core::AutoPtr<Value> Value::optNot(core::AutoPtr<Context> ctx) {

  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "inversion");
    if (opt->getType(ctx) != Atom::Type::NIL) {
      return opt->call(ctx, {this})[0];
    }
  }
  if (getType(ctx) != Atom::Type::NUMBER) {
    throw std::runtime_error(fmt::format(
        "attempt to perform arithmetic on a '{}' value ", getTypeName(ctx)));
  }
  return ctx->createValue()->setNumber(ctx, ~(uint32_t)toNumber(ctx));
}

Atom *Value::getAtom() { return _atom; }