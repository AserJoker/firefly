#pragma once
#include "JSAllocator.hpp"
#include <cstddef>
#include <string>
#include <unordered_map>
class JSRef {
public:
  static inline auto &registry() {
    static std::unordered_map<std::string, JSRef *> store;
    return store;
  }

private:
  size_t _ref;
  JSAllocator *_allocator;

public:
  JSRef(JSAllocator *allocator) : _ref(0), _allocator(allocator) {}

  virtual ~JSRef() {
    for (auto &[key, item] : registry()) {
      if (item == this) {
        registry().erase(key);
        return;
      }
    }
  }

  JSAllocator *getAllocator() const {
    return const_cast<JSAllocator *>(_allocator);
  }

  size_t addRef() { return ++_ref; }

  size_t dispose() { return --_ref; }

  size_t ref() const { return _ref; }

  void release() {
    if (!dispose()) {
      _allocator->dispose(this);
    }
  }
};