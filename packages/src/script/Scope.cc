#include "script/Scope.hpp"
#include "core/AutoPtr.hpp"
#include "script/Atom.hpp"
#include "script/Record.hpp"
#include <vector>
using namespace firefly;
using namespace firefly::script;
Scope::Scope(Scope *parent) : _parent(parent), _root(0) {
  _root = new Atom();
  _root->_type = Atom::Type::OBJECT;
  _root->_value = core::AutoPtr(new Record(_root));
  if (_parent) {
    parent->_children.push_back(this);
    _root->_parent.push_back(parent->_root);
    parent->_root->_children.push_back(_root);
  }
}
Scope::~Scope() {}

Atom *Scope::getRoot() { return _root; }
Scope *Scope::getParent() { return _parent; }
void Scope::removeChild(core::AutoPtr<Scope> scope) {
  std::erase(_children, scope);
}
std::vector<core::AutoPtr<Scope>> &Scope::getChildren() { return _children; }