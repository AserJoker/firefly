#pragma once
#include "Scope.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <unordered_map>
namespace firefly::script {
class Value;
class Context : public core::Object {
public:
  class Bridge : public core::Object {
  public:
    virtual std::vector<core::AutoPtr<Value>>
    eval(core::AutoPtr<Context> ctx, const std::string &filename,
         const std::string &source) = 0;

    virtual core::AutoPtr<Value> getGlobal(core::AutoPtr<Context> ctx) = 0;

    virtual void registerModule(
        core::AutoPtr<Context> ctx, const std::string &name,
        std::unordered_map<std::string, core::AutoPtr<Value>> exports) = 0;
  };

private:
  core::AutoPtr<Scope> _root;
  core::AutoPtr<Scope> _current;
  core::AutoPtr<Bridge> _bridge;

  static bool checkAlived(Atom *atom,
                          const std::unordered_map<ptrdiff_t, Atom *> &alived);

public:
  Context();
  ~Context() override;
  void dispose() override;
  void setBridge(core::AutoPtr<Bridge> bridge);
  core::AutoPtr<Value> getGlobal();
  core::AutoPtr<Bridge> getBridge();
  core::AutoPtr<Value> eval(const std::string &filename,
                            const std::string &source);
  core::AutoPtr<Value> createValue(Atom *atom = nullptr);
  core::AutoPtr<Scope> pushScope();
  void popScope(core::AutoPtr<Scope> scope);
  core::AutoPtr<Scope> getCurrentScope();
  core::AutoPtr<Scope> getRootScope();
  static void gc(core::AutoPtr<Context> ctx, Atom *atom);
  void store(const std::string &name, core::AutoPtr<Value> value);
  core::AutoPtr<Value> query(const std::string &name);
};
}; // namespace firefly::script