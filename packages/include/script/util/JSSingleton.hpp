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
    std::string name = typeid(T).name();
    if (!store.contains(name)) {
      auto ins = allocator->create<T>();
      store[name] = ins;
    }
    auto ins = dynamic_cast<T *>(store.at(name));
    return ins;
  }

  template <class T> static T *query() {
    auto &store = JSRef::registry();
    std::string name = typeid(T).name();
    if (!store.contains(name)) {
      return nullptr;
    }
    auto ins = dynamic_cast<T *>(store.at(name));
    return ins;
  }
};