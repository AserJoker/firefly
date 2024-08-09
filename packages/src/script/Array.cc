#include "script/Array.hpp"
#include "core/AutoPtr.hpp"
#include "script/Atom.hpp"
#include "script/Context.hpp"
#include "script/Value.hpp"
using namespace firefly;
using namespace firefly::script;
Array::Array(Atom *atom) : _self(atom) {}
void Array::setIndex(core::AutoPtr<Context> ctx, int index,
                     core::AutoPtr<Value> value) {
  if (value->getType(ctx) != Atom::Type::NIL) {
    auto atom = value->getAtom();
    atom->addParent(_self);
    while (index >= _items.size()) {
      _items.push_back(nullptr);
    }
    if (_items[index]) {
      _items[index]->removeParent(_self);
    }
    _items[index] = value->getAtom();
  } else {
    if (index < _items.size()) {
      _items[index]->removeParent(_self);
      _items[index] = nullptr;
    }
  }
  while (*_items.rbegin() == nullptr) {
    _items.pop_back();
  }
}
core::AutoPtr<Value> Array::getIndex(core::AutoPtr<Context> ctx,
                                     uint32_t index) {
  if (index >= _items.size()) {
    return ctx->createValue();
  } else {
    if (_items[index]) {
      return ctx->createValue(_items[index]);
    } else {
      return ctx->createValue();
    }
  }
}
uint32_t Array::getLength(core::AutoPtr<Context> ctx) { return _items.size(); }