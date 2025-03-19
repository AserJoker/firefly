#include "script/engine/JSAtom.hpp"
#include <algorithm>
JSAtom::JSAtom(JSAllocator *allocator, JSAtom *parent, JSBase *data)
    : _allocator(allocator), _data(data) {
  if (parent) {
    parent->addChild(this);
  }
  if (_data) {
    _data->addRef();
  }
}
JSAtom::JSAtom(JSAllocator *allocator)
    : _allocator(allocator), _data(nullptr) {}

JSAtom::~JSAtom() {
  if (_data != nullptr) {
    _data->release();
    _data = nullptr;
  }
  while (!_children.empty()) {
    removeChild(*_children.begin());
  }
  _allocator = nullptr;
}

void JSAtom::addChild(JSAtom *child) {
  _children.push_back(child);
  child->_parents.push_back(this);
}

void JSAtom::removeChild(JSAtom *child) {
  auto it = std::find(_children.begin(), _children.end(), child);
  if (it != _children.end()) {
    _children.erase(it);
  }
  it = std::find(child->_parents.begin(), child->_parents.end(), this);
  if (it != child->_parents.end()) {
    child->_parents.erase(it);
  }
  JSAtom::_destroyed.push_back(child);
}

void JSAtom::setData(JSBase *data) {
  if (_data == data) {
    return;
  }
  if (_data) {
    _data->release();
  }
  _data = data;
  _data->addRef();
}

void JSAtom::gc(JSAllocator *allocator) {
  std::vector<JSAtom *> disposed = {};
  while (!_destroyed.empty()) {
    auto item = *_destroyed.rbegin();
    _destroyed.pop_back();
    if (std::find(disposed.begin(), disposed.end(), item) != disposed.end()) {
      continue;
    }
    std::vector<JSAtom *> workflow = {item};
    std::vector<JSAtom *> cache = {};
    for (;;) {
      if (workflow.empty()) {
        disposed.push_back(item);
        while (!item->_children.empty()) {
          auto child = *item->_children.begin();
          item->removeChild(child);
        }
        while (!item->_parents.empty()) {
          auto parent = *item->_parents.begin();
          std::erase(parent->_children, item);
          item->_parents.erase(item->_parents.begin());
        }
        break;
      }
      auto it = *workflow.begin();
      workflow.erase(workflow.begin());
      if (std::find(cache.begin(), cache.end(), it) != cache.end()) {
        continue;
      }
      cache.push_back(it);
      bool alived = false;
      for (auto parent : it->_parents) {
        if (parent->_data == nullptr) {
          alived = true;
          break;
        }
        workflow.push_back(parent);
      }
      if (alived) {
        break;
      }
    }
  }
  for (auto dis : disposed) {
    allocator->dispose(dis);
  }
}