#pragma once
#include "Atom.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <vector>
namespace firefly::script {
class Scope : public core::Object {
private:
  Scope *_parent;
  std::vector<core::AutoPtr<Scope>> _children;
  Atom *_root;

public:
  Scope(Scope *parent = nullptr);
  ~Scope() override;
  Atom *getRoot();
  Scope *getParent();
  void removeChild(core::AutoPtr<Scope> scope);
  std::vector<core::AutoPtr<Scope>> &getChildren();
};
} // namespace firefly::script