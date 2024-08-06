#pragma once
#include "Runtime.hpp"
#include "Scope.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <unordered_map>
namespace firefly::script {
class Value;
class Context : public core::Object {

private:
  core::AutoPtr<Runtime> _runtime;
  core::AutoPtr<Scope> _root;
  core::AutoPtr<Scope> _current;

  static bool checkAlived(Atom *atom,
                          const std::unordered_map<ptrdiff_t, Atom *> &alived);

public:
  Context(core::AutoPtr<Runtime> rt, core::AutoPtr<Scope> scope = nullptr);
  ~Context() override;
  core::AutoPtr<Runtime> getRuntime();
  core::AutoPtr<Value> getGlobal();
  core::AutoPtr<Value> eval(const std::string &filename,
                            const std::string &source);
  core::AutoPtr<Value> createValue(Atom *atom = nullptr);
  core::AutoPtr<Scope> pushScope();
  void popScope(core::AutoPtr<Scope> scope);
  core::AutoPtr<Scope> getCurrentScope();
  core::AutoPtr<Scope> getRootScope();
  static void gc(core::AutoPtr<Context> ctx, Atom *atom);
};
}; // namespace firefly::script