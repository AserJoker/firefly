#pragma once

#include <cstddef>
#include <typeinfo>
class JSAllocator {
public:
  JSAllocator() {}

  virtual ~JSAllocator() {}

  virtual void *alloc(size_t size) { return ::operator new(size); }

  virtual void *alloc(size_t size, const std::type_info &info) {
    return alloc(size);
  }

  virtual void free(void *buf) { ::operator delete(buf); }

  virtual void free(void *buf, const std::type_info &info) { free(buf); }

  virtual void dispose() { delete this; }

  template <class T, class... ARGS> T *create(ARGS... args) {
    auto buf = alloc(sizeof(T), typeid(T));
    new (buf) T(this, args...);
    return (T *)buf;
  }

  template <class T> void dispose(T *object) {
    object->~T();
    free(object);
  }
};