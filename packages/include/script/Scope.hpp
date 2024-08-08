#pragma once
#include "Atom.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <unordered_map>
#include <vector>
namespace firefly::script {
class Value;
class Scope : public core::Object {
private:
  Scope *_parent;
  std::vector<core::AutoPtr<Scope>> _children;
  Atom *_root;
  std::unordered_map<std::string, core::AutoPtr<Value>> _storage;

public:
  Scope(Scope *parent = nullptr);
  Scope(Atom *atom);
  ~Scope() override;
  Atom *getRoot();
  Scope *getParent();
  void removeChild(core::AutoPtr<Scope> scope);
  std::vector<core::AutoPtr<Scope>> &getChildren();
  void store(const std::string &name, core::AutoPtr<Value> value);
  core::AutoPtr<Value> load(const std::string &name);
};
} // namespace firefly::script