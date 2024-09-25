#include "script/Value.hpp"
#include "core/AutoPtr.hpp"
#include "exception/JSONException.hpp"
#include "exception/ScriptException.hpp"
#include "exception/ValidateException.hpp"
#include "script/Array.hpp"
#include "script/Atom.hpp"
#include "script/Record.hpp"
#include "script/Script.hpp"
#include <any>
#include <cjson/cJSON.h>
#include <fmt/core.h>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <yaml-cpp/node/type.h>
#include <yaml-cpp/parser.h>
#include <yaml-cpp/yaml.h>

using namespace firefly;
using namespace firefly::script;
Value::Value(Atom *atom) : _atom(atom) {}
Atom::TYPE Value::getType(core::AutoPtr<Script> ctx) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto valueOf = metadata->getField(ctx, "typeof");
    if (valueOf->getType(ctx) != Atom::TYPE::NIL) {
      auto value = valueOf->call(ctx, {this})[0];
      return (Atom::TYPE)value->toNumber(ctx);
    }
  }
  return _atom->_type;
}

std::string Value::getTypeName(core::AutoPtr<Script> ctx) {
  switch (getType(ctx)) {
  case Atom::TYPE::NIL:
    return "Nil";
  case Atom::TYPE::NUMBER:
    return "Number";
  case Atom::TYPE::BOOLEAN:
    return "Boolean";
  case Atom::TYPE::STRING:
    return "String";
  case Atom::TYPE::OBJECT:
    return "Object";
  case Atom::TYPE::ARRAY:
    return "Array";
  case Atom::TYPE::FUNCTION:
    return "Function";
  }
  return "Unknown";
}
double Value::toNumber(core::AutoPtr<Script> ctx) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto valueOf = metadata->getField(ctx, "valueOf");
    if (valueOf->getType(ctx) != Atom::TYPE::NIL) {
      auto value = valueOf->call(ctx, {this})[0];
      if (value != this) {
        return value->toNumber(ctx);
      }
    }
  }
  if (getType(ctx) == Atom::TYPE::NUMBER) {
    return std::any_cast<double>(_atom->_value);
  }
  return 0;
}
bool Value::toBoolean(core::AutoPtr<Script> ctx) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto valueOf = metadata->getField(ctx, "valueOf");
    if (valueOf->getType(ctx) != Atom::TYPE::NIL) {
      auto value = valueOf->call(ctx, {this})[0];
      if (value != this) {
        return value->toBoolean(ctx);
      }
    }
  }
  if (getType(ctx) == Atom::TYPE::BOOLEAN) {
    return std::any_cast<bool>(_atom->_value);
  }
  return false;
}
std::string Value::toString(core::AutoPtr<Script> ctx) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto valueOf = metadata->getField(ctx, "valueOf");
    if (valueOf->getType(ctx) != Atom::TYPE::NIL) {
      auto value = valueOf->call(ctx, {this})[0];
      if (value != this) {
        return value->toString(ctx);
      }
    }
  }
  if (getType(ctx) == Atom::TYPE::STRING) {
    return std::any_cast<std::string>(_atom->_value);
  }
  return "";
}
core::AutoPtr<Value> Value::setNumber(core::AutoPtr<Script> ctx,
                                      const double &value) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto ofValue = metadata->getField(ctx, "ofValue");
    if (ofValue->getType(ctx) != Atom::TYPE::NIL) {
      return ofValue->call(
          ctx, {this, ctx->createValue()->setNumber(ctx, value)})[0];
    }
  }
  _atom->_type = Atom::TYPE::NUMBER;
  _atom->_value = double(value);
  return this;
}
core::AutoPtr<Value> Value::setBoolean(core::AutoPtr<Script> ctx, bool value) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto ofValue = metadata->getField(ctx, "ofValue");
    if (ofValue->getType(ctx) != Atom::TYPE::NIL) {
      return ofValue->call(
          ctx, {this, ctx->createValue()->setBoolean(ctx, value)})[0];
    }
  }
  _atom->_type = Atom::TYPE::BOOLEAN;
  _atom->_value = bool(value);
  return this;
}
core::AutoPtr<Value> Value::setString(core::AutoPtr<Script> ctx,
                                      const std::string &value) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto ofValue = metadata->getField(ctx, "ofValue");
    if (ofValue->getType(ctx) != Atom::TYPE::NIL) {
      return ofValue->call(
          ctx, {this, ctx->createValue()->setString(ctx, value)})[0];
    }
  }
  _atom->_type = Atom::TYPE::STRING;
  _atom->_value = std::string(value);
  return this;
}
core::AutoPtr<Value> Value::setNil(core::AutoPtr<Script> ctx) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto ofValue = metadata->getField(ctx, "ofValue");
    if (ofValue->getType(ctx) != Atom::TYPE::NIL) {
      return ofValue->call(ctx, {this, ctx->createValue()})[0];
    }
  }
  _atom->_type = Atom::TYPE::NIL;
  _atom->_value = nullptr;
  return this;
}
core::AutoPtr<Value> Value::setObject(core::AutoPtr<Script> ctx) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto ofValue = metadata->getField(ctx, "ofValue");
    if (ofValue->getType(ctx) != Atom::TYPE::NIL) {
      return ofValue->call(ctx, {this, ctx->createValue()->setObject(ctx)})[0];
    }
  }
  _atom->_type = Atom::TYPE::OBJECT;
  _atom->_value = core::AutoPtr(new Record(_atom));
  return this;
}
core::AutoPtr<Value> Value::setArray(core::AutoPtr<Script> ctx) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto ofValue = metadata->getField(ctx, "ofValue");
    if (ofValue->getType(ctx) != Atom::TYPE::NIL) {
      return ofValue->call(ctx, {this, ctx->createValue()->setArray(ctx)})[0];
    }
  }
  _atom->_type = Atom::TYPE::ARRAY;
  _atom->_value = core::AutoPtr(new Array(_atom));
  return this;
}
core::AutoPtr<Value> Value::setFunction(core::AutoPtr<Script> ctx,
                                        const FunctionHandle &func) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto ofValue = metadata->getField(ctx, "ofValue");
    if (ofValue->getType(ctx) != Atom::TYPE::NIL) {
      return ofValue->call(
          ctx, {this, ctx->createValue()->setFunction(ctx, func)})[0];
    }
  }
  _atom->_type = Atom::TYPE::FUNCTION;
  _atom->_value = FunctionHandle(func);
  return this;
}
std::vector<std::string> Value::getKeys(core::AutoPtr<Script> ctx) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto keys = metadata->getField(ctx, "keys");
    if (keys->getType(ctx) != Atom::TYPE::NIL) {
      std::vector<std::string> result;
      auto res = keys->call(ctx, {this})[0];
      for (uint32_t i = 0; i < res->getLength(ctx); i++) {
        result.push_back(res->getIndex(ctx, i)->toString(ctx));
      }
      return result;
    }
  }
  if (_atom->_type != Atom::TYPE::OBJECT) {
    throw exception::ScriptException("cannot get keys without object");
  }
  return std::any_cast<core::AutoPtr<Record>>(_atom->_value)->getKeys();
}

Value::Stack Value::call(core::AutoPtr<Script> ctx, Value::Stack args) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto call = metadata->getField(ctx, "call");
    if (call->getType(ctx) != Atom::TYPE::NIL) {
      Value::Stack stack = {this};
      for (auto &arg : args) {
        stack.push_back(arg);
      }
      return call->call(ctx, stack);
    }
  }
  if (_atom->_type != Atom::TYPE::FUNCTION) {
    throw exception::ScriptException("cannot call without function");
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

core::AutoPtr<Value> Value::setField(core::AutoPtr<Script> ctx,
                                     const std::string &name,
                                     core::AutoPtr<Value> field) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto setter = metadata->getField(ctx, "set");
    if (setter->getType(ctx) != Atom::TYPE::NIL) {
      setter->call(ctx,
                   {this, ctx->createValue()->setString(ctx, name), field});
      return this;
    }
  }
  if (_atom->_type != Atom::TYPE::OBJECT) {
    throw exception::ScriptException("cannot set field without object");
  }
  std::any_cast<core::AutoPtr<Record>>(_atom->_value)
      ->setField(ctx, name, field);
  return this;
}
core::AutoPtr<Value> Value::getField(core::AutoPtr<Script> ctx,
                                     const std::string &name) {

  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto getter = metadata->getField(ctx, "get");
    if (getter->getType(ctx) != Atom::TYPE::NIL) {
      return getter->call(ctx,
                          {this, ctx->createValue()->setString(ctx, name)})[0];
    }
  }
  if (_atom->_type != Atom::TYPE::OBJECT) {
    throw exception::ScriptException("cannot get field without object");
  }
  auto res =
      std::any_cast<core::AutoPtr<Record>>(_atom->_value)->getField(ctx, name);
  return res;
}
core::AutoPtr<Value> Value::setIndex(core::AutoPtr<Script> ctx, uint32_t name,
                                     core::AutoPtr<Value> field) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto setter = metadata->getField(ctx, "set");
    if (setter->getType(ctx) != Atom::TYPE::NIL) {
      setter->call(ctx,
                   {this, ctx->createValue()->setNumber(ctx, name), field});
      return this;
    }
  }
  if (getType(ctx) == Atom::TYPE::OBJECT) {
    std::any_cast<core::AutoPtr<Record>>(_atom->_value)
        ->setField(ctx, std::to_string(name), field);
  } else {
    std::any_cast<core::AutoPtr<Array>>(_atom->_value)
        ->setIndex(ctx, name, field);
  }
  return this;
}
core::AutoPtr<Value> Value::getIndex(core::AutoPtr<Script> ctx,
                                     const uint32_t &name) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto getter = metadata->getField(ctx, "get");
    if (getter->getType(ctx) != Atom::TYPE::NIL) {
      return getter->call(ctx,
                          {this, ctx->createValue()->setNumber(ctx, name)})[0];
    }
  }

  if (getType(ctx) == Atom::TYPE::OBJECT) {
    return std::any_cast<core::AutoPtr<Record>>(_atom->_value)
        ->getField(ctx, std::to_string(name));
  } else {
    return std::any_cast<core::AutoPtr<Array>>(_atom->_value)
        ->getIndex(ctx, name);
  }
}
uint32_t Value::getLength(core::AutoPtr<Script> ctx) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto length = metadata->getField(ctx, "length");
    if (length->getType(ctx) != Atom::TYPE::NIL) {
      return (uint32_t)length->call(ctx, {this})[0]->toNumber(ctx);
    }
  }
  if (getType(ctx) == Atom::TYPE::OBJECT) {
    return std::any_cast<core::AutoPtr<Record>>(_atom->_value)->getLength(ctx);
  }
  if (getType(ctx) == Atom::TYPE::ARRAY) {
    return std::any_cast<core::AutoPtr<Array>>(_atom->_value)->getLength(ctx);
  }
  return 0;
}
core::AutoPtr<Value> Value::getMetadata(core::AutoPtr<Script> ctx) {
  return ctx->createValue(_atom->_metadata);
}
core::AutoPtr<Value> Value::setMetadata(core::AutoPtr<Script> ctx,
                                        core::AutoPtr<Value> value) {
  if (_atom->_metadata == value->getAtom()) {
    return this;
  }
  auto metadata = value->getAtom();
  std::unordered_map<ptrdiff_t, Atom *> cache;
  while (metadata) {
    if (cache.contains((ptrdiff_t)metadata)) {
      throw exception::ScriptException(
          "Failed to set metadata: cycle metadata");
    }
    cache[(ptrdiff_t)metadata] = metadata;
    metadata = metadata->_metadata;
  }
  metadata = value->getAtom();
  if (_atom->_metadata) {
    _atom->_metadata->addParent(ctx->getCurrentScope()->getRoot());
    _atom->_metadata->removeParent(_atom);
  }
  _atom->_metadata = metadata;
  if (metadata) {
    metadata->addParent(_atom);
  }
  return this;
}

core::AutoPtr<Value> Value::setRawField(core::AutoPtr<Script> ctx,
                                        const std::string &name,
                                        core::AutoPtr<Value> field) {
  if (_atom->_type != Atom::TYPE::OBJECT) {
    throw exception::ScriptException("cannot set raw field without object");
  }
  std::any_cast<core::AutoPtr<Record>>(_atom->_value)
      ->setField(ctx, name, field);
  return this;
}
core::AutoPtr<Value> Value::getRawField(core::AutoPtr<Script> ctx,
                                        const std::string &name) {
  if (_atom->_type != Atom::TYPE::OBJECT) {
    throw exception::ScriptException("cannot get raw field without object");
  }
  return std::any_cast<core::AutoPtr<Record>>(_atom->_value)
      ->getField(ctx, name);
}
core::AutoPtr<Value> Value::setRawIndex(core::AutoPtr<Script> ctx,
                                        const uint32_t &name,
                                        core::AutoPtr<Value> field) {
  if (_atom->_type != Atom::TYPE::ARRAY) {
    throw exception::ScriptException("cannot set raw index without array");
  }
  std::any_cast<core::AutoPtr<Array>>(_atom->_value)
      ->setIndex(ctx, name, field);
  return this;
}
core::AutoPtr<Value> Value::getRawIndex(core::AutoPtr<Script> ctx,
                                        const uint32_t &name) {

  if (_atom->_type != Atom::TYPE::ARRAY) {
    throw exception::ScriptException("cannot get raw index without array");
  }
  auto res =
      std::any_cast<core::AutoPtr<Array>>(_atom->_value)->getIndex(ctx, name);
  return res;
}
uint32_t Value::getRawLength(core::AutoPtr<Script> ctx) {
  if (_atom->_type == Atom::TYPE::OBJECT) {
    return std::any_cast<core::AutoPtr<Record>>(_atom->_value)->getLength(ctx);
  } else if (_atom->_type == Atom::TYPE::ARRAY) {
    return std::any_cast<core::AutoPtr<Array>>(_atom->_value)->getLength(ctx);
  } else {
    throw exception::ScriptException(
        "cannot get raw length without object or array");
  }
}

core::AutoPtr<Value> Value::arithmetic(core::AutoPtr<Script> ctx,
                                       core::AutoPtr<Value> another,
                                       OPERATION OPERATION) {
  switch (OPERATION) {
  case OPERATION::CONNECT:
    return optConnect(ctx, another);
  case OPERATION::ADD:
    return optAdd(ctx, another);
  case OPERATION::SUB:
    return optSub(ctx, another);
  case OPERATION::MUL:
    return optMul(ctx, another);
  case OPERATION::DIV:
    return optDiv(ctx, another);
  case OPERATION::MOD:
    return optMod(ctx, another);
  case OPERATION::EQ:
    return optEq(ctx, another);
  case OPERATION::NE:
    return optNe(ctx, another);
  case OPERATION::GT:
    return optGt(ctx, another);
  case OPERATION::GE:
    return optGe(ctx, another);
  case OPERATION::LT:
    return optLt(ctx, another);
  case OPERATION::LE:
    return optLe(ctx, another);
  case OPERATION::AND:
    return optAnd(ctx, another);
  case OPERATION::OR:
    return optOr(ctx, another);
  case OPERATION::XOR:
    return optXor(ctx, another);
  case OPERATION::NOT:
    return optNot(ctx);
  case OPERATION::SHR:
    return optShr(ctx, another);
  case OPERATION::SHL:
    return optShl(ctx, another);
  }
  return nullptr;
}

core::AutoPtr<Value> Value::optConnect(core::AutoPtr<Script> ctx,
                                       core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "connect");
    if (opt->getType(ctx) != Atom::TYPE::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::TYPE::STRING ||
      another->getType(ctx) != Atom::TYPE::STRING) {
    throw exception::ValidateException(fmt::format(
        "attempt to perform connect between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto source = toString(ctx);
  auto target = another->toString(ctx);
  return ctx->createValue()->setString(ctx, source + target);
}
core::AutoPtr<Value> Value::optAdd(core::AutoPtr<Script> ctx,
                                   core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "add");
    if (opt->getType(ctx) != Atom::TYPE::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::TYPE::NUMBER ||
      another->getType(ctx) != Atom::TYPE::NUMBER) {
    throw exception::ValidateException(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, left + right);
}
core::AutoPtr<Value> Value::optSub(core::AutoPtr<Script> ctx,
                                   core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "subtract");
    if (opt->getType(ctx) != Atom::TYPE::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::TYPE::NUMBER ||
      another->getType(ctx) != Atom::TYPE::NUMBER) {
    throw exception::ValidateException(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, left - right);
}
core::AutoPtr<Value> Value::optMul(core::AutoPtr<Script> ctx,
                                   core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "times");
    if (opt->getType(ctx) != Atom::TYPE::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::TYPE::NUMBER ||
      another->getType(ctx) != Atom::TYPE::NUMBER) {
    throw exception::ValidateException(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, left * right);
}
core::AutoPtr<Value> Value::optDiv(core::AutoPtr<Script> ctx,
                                   core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "over");
    if (opt->getType(ctx) != Atom::TYPE::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::TYPE::NUMBER ||
      another->getType(ctx) != Atom::TYPE::NUMBER) {
    throw exception::ValidateException(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, left / right);
}
core::AutoPtr<Value> Value::optMod(core::AutoPtr<Script> ctx,
                                   core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "module");
    if (opt->getType(ctx) != Atom::TYPE::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::TYPE::NUMBER ||
      another->getType(ctx) != Atom::TYPE::NUMBER) {
    throw exception::ValidateException(fmt::format(
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
core::AutoPtr<Value> Value::optAnd(core::AutoPtr<Script> ctx,
                                   core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "and");
    if (opt->getType(ctx) != Atom::TYPE::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::TYPE::NUMBER ||
      another->getType(ctx) != Atom::TYPE::NUMBER) {
    throw exception::ValidateException(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, (uint32_t)left & (uint32_t)right);
}
core::AutoPtr<Value> Value::optOr(core::AutoPtr<Script> ctx,
                                  core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "or");
    if (opt->getType(ctx) != Atom::TYPE::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::TYPE::NUMBER ||
      another->getType(ctx) != Atom::TYPE::NUMBER) {
    throw exception::ValidateException(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, (uint32_t)left | (uint32_t)right);
}
core::AutoPtr<Value> Value::optXor(core::AutoPtr<Script> ctx,
                                   core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "xor");
    if (opt->getType(ctx) != Atom::TYPE::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::TYPE::NUMBER ||
      another->getType(ctx) != Atom::TYPE::NUMBER) {
    throw exception::ValidateException(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, (uint32_t)left ^ (uint32_t)right);
}
core::AutoPtr<Value> Value::optEq(core::AutoPtr<Script> ctx,
                                  core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "equal");
    if (opt->getType(ctx) != Atom::TYPE::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != another->getType(ctx)) {
    return ctx->createValue()->setBoolean(ctx, false);
  }
  switch (getType(ctx)) {
  case Atom::TYPE::NIL:
    return ctx->createValue()->setBoolean(ctx, true);
  case Atom::TYPE::NUMBER:
    return ctx->createValue()->setBoolean(ctx, toNumber(ctx) ==
                                                   another->toNumber(ctx));
  case Atom::TYPE::BOOLEAN:
    return ctx->createValue()->setBoolean(ctx, toBoolean(ctx) ==
                                                   another->toBoolean(ctx));
  case Atom::TYPE::STRING:
    return ctx->createValue()->setBoolean(ctx, toString(ctx) ==
                                                   another->toString(ctx));
  default:
    return ctx->createValue()->setBoolean(ctx, _atom == another->_atom);
  }
}
core::AutoPtr<Value> Value::optNe(core::AutoPtr<Script> ctx,
                                  core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "notEqual");
    if (opt->getType(ctx) != Atom::TYPE::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != another->getType(ctx)) {
    return ctx->createValue()->setBoolean(ctx, false);
  }

  switch (getType(ctx)) {
  case Atom::TYPE::NIL:
    return ctx->createValue()->setBoolean(ctx, false);
  case Atom::TYPE::NUMBER:
    return ctx->createValue()->setBoolean(ctx, toNumber(ctx) !=
                                                   another->toNumber(ctx));
  case Atom::TYPE::BOOLEAN:
    return ctx->createValue()->setBoolean(ctx, toBoolean(ctx) !=
                                                   another->toBoolean(ctx));
  case Atom::TYPE::STRING:
    return ctx->createValue()->setBoolean(ctx, toString(ctx) !=
                                                   another->toString(ctx));
  default:
    return ctx->createValue()->setBoolean(ctx, _atom != another->_atom);
  }
}
core::AutoPtr<Value> Value::optGt(core::AutoPtr<Script> ctx,
                                  core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "greaterThan");
    if (opt->getType(ctx) != Atom::TYPE::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::TYPE::NUMBER ||
      another->getType(ctx) != Atom::TYPE::NUMBER) {
    throw exception::ValidateException(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, left > right);
}
core::AutoPtr<Value> Value::optGe(core::AutoPtr<Script> ctx,
                                  core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "greaterAndEqual");
    if (opt->getType(ctx) != Atom::TYPE::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::TYPE::NUMBER ||
      another->getType(ctx) != Atom::TYPE::NUMBER) {
    throw exception::ValidateException(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, left >= right);
}
core::AutoPtr<Value> Value::optLt(core::AutoPtr<Script> ctx,
                                  core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "lessThan");
    if (opt->getType(ctx) != Atom::TYPE::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::TYPE::NUMBER ||
      another->getType(ctx) != Atom::TYPE::NUMBER) {
    throw exception::ValidateException(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, left < right);
}
core::AutoPtr<Value> Value::optLe(core::AutoPtr<Script> ctx,
                                  core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "lessAndEqual");
    if (opt->getType(ctx) != Atom::TYPE::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::TYPE::NUMBER ||
      another->getType(ctx) != Atom::TYPE::NUMBER) {
    throw exception::ValidateException(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, left <= right);
}
core::AutoPtr<Value> Value::optShl(core::AutoPtr<Script> ctx,
                                   core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "leftShifting");
    if (opt->getType(ctx) != Atom::TYPE::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::TYPE::NUMBER ||
      another->getType(ctx) != Atom::TYPE::NUMBER) {
    throw exception::ValidateException(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, (uint32_t)left << (uint32_t)right);
}
core::AutoPtr<Value> Value::optShr(core::AutoPtr<Script> ctx,
                                   core::AutoPtr<Value> another) {
  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "rightShifting");
    if (opt->getType(ctx) != Atom::TYPE::NIL) {
      return opt->call(ctx, {this, another})[0];
    }
  }
  if (getType(ctx) != Atom::TYPE::NUMBER ||
      another->getType(ctx) != Atom::TYPE::NUMBER) {
    throw exception::ValidateException(fmt::format(
        "attempt to perform arithmetic between a '{}' value and a '{}' value",
        getTypeName(ctx), another->getTypeName(ctx)));
  }
  auto left = toNumber(ctx);
  auto right = toNumber(ctx);
  return ctx->createValue()->setNumber(ctx, (uint32_t)left >> (uint32_t)right);
}
core::AutoPtr<Value> Value::optNot(core::AutoPtr<Script> ctx) {

  if (_atom->_metadata) {
    auto metadata = getMetadata(ctx);
    auto opt = metadata->getField(ctx, "inversion");
    if (opt->getType(ctx) != Atom::TYPE::NIL) {
      return opt->call(ctx, {this})[0];
    }
  }
  if (getType(ctx) != Atom::TYPE::NUMBER) {
    throw exception::ValidateException(fmt::format(
        "attempt to perform arithmetic on a '{}' value ", getTypeName(ctx)));
  }
  return ctx->createValue()->setNumber(ctx, ~(int32_t)toNumber(ctx));
}

Atom *Value::getAtom() { return _atom; }
core::AutoPtr<Value> Value::setOpaque(core::AutoPtr<core::Object> obj) {
  _atom->_opaque = obj;
  return this;
}
core::AutoPtr<core::Object> Value::getOpaque() { return _atom->_opaque; }

static cJSON *toJSONItem(core::AutoPtr<Script> ctx,
                         core::AutoPtr<Value> value) {
  cJSON *root = nullptr;
  switch (value->getType(ctx)) {
  case Atom::TYPE::NIL:
    root = cJSON_CreateNull();
    break;
  case Atom::TYPE::NUMBER:
    root = cJSON_CreateNumber(value->toNumber(ctx));
    break;
  case Atom::TYPE::BOOLEAN:
    root = cJSON_CreateBool(value->toBoolean(ctx));
    break;
  case Atom::TYPE::STRING:
    root = cJSON_CreateString(value->toString(ctx).c_str());
    break;
  case Atom::TYPE::OBJECT:
    root = cJSON_CreateObject();
    for (auto &key : value->getKeys(ctx)) {
      cJSON_AddItemToObject(root, key.c_str(),
                            toJSONItem(ctx, value->getField(ctx, key)));
    }
    break;
  case Atom::TYPE::ARRAY: {
    root = cJSON_CreateArray();
    auto length = value->getLength(ctx);
    for (uint32_t i = 0; i < length; i++) {
      cJSON_AddItemToArray(root, toJSONItem(ctx, value->getIndex(ctx, i)));
    }
  } break;
  case Atom::TYPE::FUNCTION:
    root = cJSON_CreateNull();
    break;
  }
  return root;
}
static YAML::Node toYAMLItem(core::AutoPtr<Script> ctx,
                             core::AutoPtr<Value> value) {
  YAML::Node node;
  switch (value->getType(ctx)) {
  case Atom::TYPE::NIL:
    break;
  case Atom::TYPE::NUMBER:
    node = value->toNumber(ctx);
    break;
  case Atom::TYPE::BOOLEAN:
    node = value->toBoolean(ctx);
    break;
  case Atom::TYPE::STRING:
    node = value->toString(ctx);
    break;
  case Atom::TYPE::OBJECT:
    for (auto &key : value->getKeys(ctx)) {
      node[key] = toYAMLItem(ctx, value->getField(ctx, key));
    }
    break;
  case Atom::TYPE::ARRAY: {
    auto length = value->getLength(ctx);
    for (uint32_t i = 0; i < length; i++) {
      node.push_back(toYAMLItem(ctx, value->getIndex(ctx, i)));
    }
  } break;
  case Atom::TYPE::FUNCTION:
    break;
  }
  return node;
}
std::string Value::toJSON(core::AutoPtr<Script> ctx) {
  auto json = toJSONItem(ctx, this);
  std::string result = cJSON_Print(json);
  cJSON_Delete(json);
  return result;
}
std::string Value::toYAML(core::AutoPtr<Script> ctx) {
  auto node = toYAMLItem(ctx, this);
  std::stringstream ss;
  ss << node;
  return ss.str();
}
static core::AutoPtr<Value> parseJSONItem(core::AutoPtr<Script> ctx,
                                          cJSON *root) {
  auto result = ctx->createValue();
  if (cJSON_IsObject(root)) {
    result->setObject(ctx);
    auto child = root->child;
    while (child != nullptr) {
      result->setField(ctx, child->string, parseJSONItem(ctx, child));
      child = child->next;
    }
  } else if (cJSON_IsArray(root)) {
    result->setArray(ctx);
    auto child = root->child;
    auto index = 0;
    while (child != nullptr) {
      result->setIndex(ctx, index++, parseJSONItem(ctx, child));
      child = child->next;
    }
  } else if (cJSON_IsBool(root)) {
    result->setBoolean(ctx, root->valueint != 0);
  } else if (cJSON_IsNumber(root)) {
    result->setNumber(ctx, root->valuedouble);
  } else if (cJSON_IsString(root)) {
    result->setString(ctx, root->valuestring);
  }
  return result;
}
core::AutoPtr<Value> Value::parseJSON(core::AutoPtr<Script> ctx,
                                      const std::string &source) {
  auto root = cJSON_Parse(source.c_str());
  auto error = cJSON_GetErrorPtr();
  if (error != 0) {
    cJSON_Delete(root);
    throw exception::JSONException(error);
  }
  auto res = parseJSONItem(ctx, root);
  cJSON_Delete(root);
  return res;
}
static core::AutoPtr<Value> parseYAMLItem(core::AutoPtr<Script> ctx,
                                          YAML::Node root) {
  if (root.Type() == YAML::NodeType::Scalar) {
    if (root.IsNull()) {
      return ctx->createValue();
    } else if (root.Tag() == "?") {
      auto s = root.Scalar();
      if (s[0] >= '0' && s[0] <= '9') {
        return ctx->createValue()->setNumber(ctx, root.as<float>());
      } else {
        return ctx->createValue()->setString(ctx, root.as<std::string>());
      }
    } else {
      return ctx->createValue()->setString(ctx, root.as<std::string>());
    }
  } else if (root.Type() == YAML::NodeType::Map) {
    auto obj = ctx->createValue()->setObject(ctx);
    for (auto it = root.begin(); it != root.end(); it++) {
      auto key = it->first;
      auto value = it->second;
      obj->setField(ctx, key.as<std::string>(), parseYAMLItem(ctx, value));
    }
    return obj;
  } else if (root.IsSequence()) {
    auto arr = ctx->createValue()->setArray(ctx);
    auto index = 0;
    for (auto it = root.begin(); it != root.end(); it++) {
      arr->setIndex(ctx, index++, parseYAMLItem(ctx, it->as<YAML::Node>()));
    }
    return arr;
  }
  return ctx->createValue();
}
core::AutoPtr<Value> Value::parseYAML(core::AutoPtr<Script> ctx,
                                      const std::string &source) {
  auto root = YAML::Load(source);
  return parseYAMLItem(ctx, root);
}