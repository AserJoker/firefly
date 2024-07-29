#pragma once
#include "LuaValue.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <cstddef>
#include <fmt/core.h>
#include <functional>
#include <unordered_map>
namespace firefly::script {
using LuaClassMethod = std::function<LuaValue::LuaValueStack(
    core::AutoPtr<core::Object> self, lua_State *state,
    const LuaValue::LuaValueStack &args)>;

using LuaClassConstructor = std::function<core::AutoPtr<core::Object>(
    lua_State *state, const LuaValue::LuaValueStack &args)>;

class LuaClass : public LuaValue {
private:
  static inline std::unordered_map<std::string, core::AutoPtr<core::Object>>
      _objects;

private:
  LuaClassConstructor _constructor;
  std::unordered_map<std::string, LuaCFunction> _methods;

private:
  static LuaValueStack gc(lua_State *state, const LuaValueStack &args) {
    auto self = args[0];
    auto addr = self->getField("$handle");
    if (addr->getType() == LUA_TSTRING) {
      _objects.erase(addr->toString());
    }
    return {};
  }
  static LuaValueStack newindex(lua_State *state, const LuaValueStack &args) {
    auto table = args[0];
    auto key = args[1];
    auto value = args[2];
    if (key->getType() == LUA_TSTRING || key->toString() != "$handle") {
      table->setField(key->toString(), value);
    } else if (key->getType() == LUA_TNUMBER) {
      table->setIndex(key->toInteger(), value);
    }
    return {};
  }
  static auto
  index(const std::string &identity,
        const std::unordered_map<std::string, LuaCFunction> &methods) {
    return [=](lua_State *state, const LuaValueStack &args) -> LuaValueStack {
      auto key = args[1]->toString();
      if (key == "$handle") {
        return {LuaValue::create(state, identity)};
      }
      if (methods.contains(key)) {
        return {LuaValue::create(state, methods.at(key))};
      }
      return {};
    };
  }
  LuaValueStack construct(lua_State *state,
                          const core::AutoPtr<core::Object> &obj,
                          core::AutoPtr<LuaValue> table = nullptr) {
    if (table == nullptr) {
      table = LuaValue::create(state, LuaRawObject{});
    }
    ptrdiff_t addr = (ptrdiff_t) & *obj;
    auto identity = fmt::format("[native 0x{:x}]", addr);
    LuaClass::_objects[identity] = obj;
    auto meta = LuaValue::create(state, LuaRawObject{});
    meta->setField("__index",
                   LuaValue::create(state, index(identity, _methods)));
    meta->setField("__gc",
                   LuaValue::create(state, std::function(&LuaClass::gc)));
    meta->setField("__newindex",
                   LuaValue::create(state, std::function(&LuaClass::newindex)));
    table->setMetadata(meta);
    return {table};
  }
  LuaValueStack construct(lua_State *state, const LuaValueStack &rawargs) {
    auto table = LuaValue::create(state, LuaRawObject{});
    LuaValueStack args = {table};
    for (auto &arg : rawargs) {
      args.push_back(arg);
    }
    auto obj = this->_constructor(state, args);
    return construct(state, obj, table);
  }

public:
  LuaClass(lua_State *state);
  core::AutoPtr<LuaClass> constructor(const LuaClassConstructor &handle) {
    _constructor = handle;
    return this;
  }
  core::AutoPtr<LuaClass> method(const std::string &name,
                                 const LuaClassMethod &method) {
    auto state = getLuaContext();
    auto fn = [=, this](lua_State *state,
                        const LuaValueStack &args) -> LuaValueStack {
      auto self = args[0];
      core::AutoPtr<core::Object> obj = nullptr;
      if (self->getType() != LUA_TNIL) {
        auto handle = self->getField("$handle");
        if (handle->getType() == LUA_TSTRING) {
          if (_objects.contains(handle->toString())) {
            obj = _objects.at(handle->toString());
          }
        }
      }
      return method(obj, state, args);
    };
    _methods[name] = fn;
    return this;
  }
  LuaValueStack create(lua_State *state,
                       const core::AutoPtr<core::Object> &obj) {
    return construct(state, obj);
  }
};
#define LUA_CONSTRUCTOR_DEFINE(name)                                           \
  core::AutoPtr<core::Object> name(lua_State *state,                           \
                                   const LuaValue::LuaValueStack &args)
#define LUA_METHOD_DEFINE(name)                                                \
  LuaValue::LuaValueStack name(core::AutoPtr<core::Object> self,               \
                               lua_State *state,                               \
                               const LuaValue::LuaValueStack &args)

}; // namespace firefly::script