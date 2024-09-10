#include "script/Record.hpp"
#include "script/Atom.hpp"
#include "script/Value.hpp"
#include <vector>
using namespace firefly;
using namespace firefly::script;
Record::Record(Atom *atom) : _self(atom) {}

void Record::setField(core::AutoPtr<Script> ctx, const std::string &name,
                      core::AutoPtr<Value> field) {
  if (_fields.contains(name)) {
    if (_fields[name] == field->getAtom()) {
      return;
    }
    _fields[name]->removeParent(_self);
  }
  field->getAtom()->addParent(_self);
  _fields[name] = field->getAtom();
}
core::AutoPtr<Value> Record::getField(core::AutoPtr<Script> ctx,
                                      const std::string &name) {
  if (_fields.contains(name)) {
    return ctx->createValue(_fields[name]);
  } else {
    return ctx->createValue();
  }
}
void Record::removeField(core::AutoPtr<Script> ctx, const std::string &name) {
  if (_fields.contains(name)) {
    _fields[name]->removeParent(_self);
    _fields.erase(name);
  }
}
uint32_t Record::getLength(core::AutoPtr<Script> ctx) {
  return (uint32_t)_fields.size();
}
std::vector<std::string> Record::getKeys() {
  std::vector<std::string> keys;
  for (auto &[k, _] : _fields) {
    keys.push_back(k);
  }
  return keys;
}