#pragma once
#include "Scope.hpp"
#include "Value.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <any>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace firefly::script {
struct AnyValue {
  std::any value;
  AnyValue(const std::string &value) : value(std::string(value)) {}
  AnyValue(const char *value) : value(std::string(value)) {}
  AnyValue(std::string &&value)
      : value(std::string(std::forward<std::string>(value))) {}
  AnyValue(bool value) : value(value) {}
  AnyValue(std::nullptr_t) : value(nullptr) {}
  AnyValue(const Value::FunctionHandle &value) : value(value) {}
  AnyValue(double value) : value(value) {}
  AnyValue(float value) : value(double(value)) {}
  AnyValue(uint32_t value) : value(double(value)) {}
  AnyValue(int32_t value) : value(double(value)) {}
};
using AnyRecord = std::unordered_map<std::string, AnyValue>;
using AnyArray = std::vector<AnyValue>;
class Value;
class Script : public core::Object {
public:
  class Bridge : public core::Object {
  public:
    virtual std::vector<core::AutoPtr<Value>>
    eval(const std::string &source) = 0;

    virtual core::AutoPtr<Value> getGlobal() = 0;

    virtual void registerModule(const std::string &name,
                                core::AutoPtr<Value> exports) = 0;
    virtual void gc() = 0;
    virtual void dispose() {};
  };

private:
  core::AutoPtr<Scope> _root;
  core::AutoPtr<Scope> _current;
  core::AutoPtr<Bridge> _bridge;
  std::unordered_map<ptrdiff_t, Atom *> _destroyList;

private:
  bool isAlived(Atom *atom, const std::unordered_map<ptrdiff_t, Atom *> &cache);

public:
  Script();
  ~Script() override;
  void dispose();
  void setBridge(core::AutoPtr<Bridge> bridge);
  core::AutoPtr<Value> getGlobal();
  core::AutoPtr<Value> getNativeGlobal();
  core::AutoPtr<Bridge> getBridge();
  std::vector<core::AutoPtr<Value>> eval(const std::string &source);
  core::AutoPtr<Value> createValue(Atom *atom = nullptr);
  core::AutoPtr<Value> createValue(const core::AutoPtr<Value> &value) {
    return value;
  }
  core::AutoPtr<Value> createValue(AnyValue &&value);
  core::AutoPtr<Value> createValue(const AnyValue &value);
  core::AutoPtr<Value> createValue(const std::vector<AnyValue> &value);
  core::AutoPtr<Value>
  createValue(const std::unordered_map<std::string, AnyValue> &value);
  core::AutoPtr<Scope> pushScope();
  void popScope(core::AutoPtr<Scope> scope);
  core::AutoPtr<Scope> getCurrentScope();
  core::AutoPtr<Scope> getRootScope();
  void store(const std::string &name, core::AutoPtr<Value> value);
  core::AutoPtr<Value> query(const std::string &name);
  void registerModule(const std::string &name, core::AutoPtr<Value> exports);
  void gc();

  void destroy(Atom *atom);

  template <int index, class T>
  static constexpr void parseArg(core::AutoPtr<script::Script> &ctx, T &tuple,
                                 script::Value::Stack &args) {}

  template <int index, class T, class ARG, class... ARGS>
  static constexpr void parseArg(core::AutoPtr<script::Script> &ctx, T &tuple,
                                 script::Value::Stack &args) {
    if constexpr (std::is_same_v<ARG, bool>) {
      std::get<index>(tuple) = args[index]->toBoolean(ctx);
    } else if constexpr (std::is_integral_v<ARG> ||
                         std::is_floating_point_v<ARG>) {
      std::get<index>(tuple) = (ARG)args[index]->toNumber(ctx);
    } else if constexpr (std::is_same_v<ARG, std::string>) {
      std::get<index>(tuple) = args[index]->toString(ctx);
    } else if constexpr (std::is_same_v<ARG, core::AutoPtr<script::Value>>) {
      std::get<index>(tuple) = args[index];
    } else {
      std::get<index>(tuple) =
          args[index]->getOpaque().cast<typename ARG::innerType>();
    }
    parseArg<index + 1, T, ARGS...>(ctx, tuple, args);
  }

  template <class... ARGS>
  static constexpr std::tuple<ARGS...>
  parseArgs(core::AutoPtr<script::Script> &ctx, script::Value::Stack &args) {
    std::tuple<ARGS...> result = std::make_tuple(ARGS{}...);
    parseArg<0, std::tuple<ARGS...>, ARGS...>(ctx, result, args);
    return result;
  }
};
#define FUNC_DEF(name)                                                         \
  Value::Stack name(core::AutoPtr<Script> ctx, Value::Stack args)
#define createNumber(ctx, value) ctx->createValue()->setNumber(ctx, value)
#define createString(ctx, value) ctx->createValue()->setString(ctx, value)
#define createBoolean(ctx, value) ctx->createValue()->setBoolean(ctx, value)
#define createFunction(ctx, value) ctx->createValue()->setFunction(ctx, value)
#define setFunctionField(ctx, func)                                            \
  setField(ctx, #func, ctx->createValue()->setFunction(ctx, func))
}; // namespace firefly::script