#pragma once
#include "Context.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "script/Atom.hpp"
#include <vector>
namespace firefly::script {
class Array : public core::Object {
private:
  std::vector<Atom *> _items;
  Atom *_self;

public:
  Array(Atom *self);
  uint32_t getLength();
  core::AutoPtr<Value> getIndex(core::AutoPtr<Context> ctx, uint32_t index);
  void setIndex(core::AutoPtr<Context> ctx, int index,
                core::AutoPtr<Value> item);
};
}; // namespace firefly::script