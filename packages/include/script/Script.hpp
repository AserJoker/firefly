#pragma once
#include "Scope.hpp"
#include "Value.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <unordered_map>

namespace firefly::script {
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
  core::AutoPtr<Scope> pushScope();
  void popScope(core::AutoPtr<Scope> scope);
  core::AutoPtr<Scope> getCurrentScope();
  core::AutoPtr<Scope> getRootScope();
  void store(const std::string &name, core::AutoPtr<Value> value);
  core::AutoPtr<Value> query(const std::string &name);
  void registerModule(const std::string &name, core::AutoPtr<Value> exports);
  void gc();

  void destroy(Atom *atom);
};
#define FUNC_DEF(name)                                                         \
  Value::Stack name(core::AutoPtr<Script> ctx, Value::Stack args)
#define createNumber(ctx, value) ctx->createValue()->setNumber(ctx, value)
#define createString(ctx, value) ctx->createValue()->setString(ctx, value)
#define createBoolean(ctx, value) ctx->createValue()->setBoolean(ctx, value)
#define createFunction(ctx, value) ctx->createValue()->setFunction(ctx, value)
#define setFunctionField(ctx, func)                                            \
  setField(ctx, #func, ctx->createValue()->setFunction(ctx, func))
#define VALIDATE_ARGS(func, s)                                                 \
  if (args.size() < s) {                                                       \
    throw exception::ValidateException(                                        \
        fmt::format("Failed to call '{}',this function requires {} "           \
                    "argument(s) but instead it is receiving {}",              \
                    #func, s, args.size()));                                   \
  }
}; // namespace firefly::script