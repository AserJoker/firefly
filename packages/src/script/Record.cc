#include "script/Record.hpp"
#include "script/Atom.hpp"
#include "script/Value.hpp"
#include <vector>
using namespace firefly;
using namespace firefly::script;
Record::Record(Atom *atom) : _self(atom) {}

void Record::setField(core::AutoPtr<Context> ctx, const std::string &name,
                      core::AutoPtr<Value> field) {
  if (_fields.contains(name)) {
    if (_fields[name] == field->getAtom()) {
      return;
    }
    _fields[name]->_parent.erase(std::find(
        _fields[name]->_parent.begin(), _fields[name]->_parent.end(), _self));
    _self->_children.erase(std::find(_self->_children.begin(),
                                     _self->_children.end(), _fields[name]));
  }
  field->getAtom()->_parent.push_back(_self);
  _self->_children.push_back(field->getAtom());
  _fields[name] = field->getAtom();
}
core::AutoPtr<Value> Record::getField(core::AutoPtr<Context> ctx,
                                      const std::string &name) {
  if (_fields.contains(name)) {
    return ctx->createValue(_fields[name]);
  } else {
    return ctx->createValue();
  }
}
void Record::removeField(core::AutoPtr<Context> ctx, const std::string &name) {
  if (_fields.contains(name)) {
    _fields[name]->_parent.erase(std::find(
        _fields[name]->_parent.begin(), _fields[name]->_parent.end(), _self));
    _self->_children.erase(std::find(_self->_children.begin(),
                                     _self->_children.end(), _fields[name]));
    _fields.erase(name);
  }
}
uint32_t Record::getLength(core::AutoPtr<Context> ctx) {
  return _fields.size();
}
std::vector<std::string> Record::getKeys() {
  std::vector<std::string> keys;
  for (auto &[k, _] : _fields) {
    keys.push_back(k);
  }
  return keys;
}