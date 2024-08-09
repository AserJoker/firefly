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

    virtual void registerModule(
        const std::string &name,
        std::unordered_map<std::string, core::AutoPtr<Value>> exports) = 0;
  };

private:
  core::AutoPtr<Scope> _root;
  core::AutoPtr<Scope> _current;
  core::AutoPtr<Bridge> _bridge;

  static bool checkAlived(Atom *atom,
                          const std::unordered_map<ptrdiff_t, Atom *> &alived);

public:
  Script();
  ~Script() override;
  void dispose() override;
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
  static void gc(core::AutoPtr<Script> ctx, Atom *atom);
  void store(const std::string &name, core::AutoPtr<Value> value);
  core::AutoPtr<Value> query(const std::string &name);
  void
  registerModule(const std::string &name,
                 std::unordered_map<std::string, core::AutoPtr<Value>> exports);
};
}; // namespace firefly::script