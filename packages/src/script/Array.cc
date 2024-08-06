#include "script/Array.hpp"
#include "core/AutoPtr.hpp"
#include "script/Context.hpp"
#include "script/Value.hpp"
using namespace firefly;
using namespace firefly::script;
void Array::setIndex(core::AutoPtr<Context> ctx, int index,
                     core::AutoPtr<Value> value) {
  while (index >= _items.size()) {
    Atom *nil = new Atom();
    nil->_parent.push_back(_self);
    _self->_children.push_back(nil);
    _items.push_back(nil);
  }
  
}