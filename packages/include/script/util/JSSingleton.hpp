#pragma once
#include "JSAllocator.hpp"
#include "JSRef.hpp"
#include <unordered_map>
class JSSingleton {

protected:
  JSSingleton() {}

public:
  virtual ~JSSingleton() {}

  template <class T> static T *instance(JSAllocator *allocator) {
    auto &store = JSRef::registry();
    if (!store.contains(typeid(T).name())) {
      auto ins = allocator->create<T>();
      store[typeid(T).name()] = ins;
    }
    auto ins = dynamic_cast<T *>(store.at(typeid(T).name()));
    return ins;
  }

  template <class T> static T *query() {
    auto &store = JSRef::registry();
    if (!store.contains(typeid(T).name())) {
      return nullptr;
    }
    auto ins = dynamic_cast<T *>(store.at(typeid(T).name()));
    ins->addRef();
    return ins;
  }
};