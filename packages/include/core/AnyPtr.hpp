#pragma once
#include "core/Any.hpp"
#include <corecrt_startup.h>
#include <typeinfo>
namespace firefly::core {
class AnyPtr {
private:
  struct Handle {
    virtual core::Any get(void *ptr) = 0;
    virtual void set(void *ptr, const core::Any &value) = 0;
    virtual Handle *clone() = 0;
    virtual ~Handle(){};
    virtual const std::type_info &type() = 0;
  };
  template <class T> struct HandleImpl : public Handle {
    core::Any get(void *ptr) override {
      return *(T *)ptr;
    }
    void set(void *ptr, const core::Any &value) override {
      *(T *)ptr = value.as<T>();
    }
    Handle *clone() override { return new HandleImpl<T>(); }
    const std::type_info &type() override { return typeid(T); }
  };

private:
  void *_ptr;
  Handle *_handle;

public:
  AnyPtr() : _ptr(nullptr), _handle(new HandleImpl<std::nullptr_t>) {}

  template <class T> AnyPtr(T *ptr) : _ptr(ptr), _handle(new HandleImpl<T>) {}

  AnyPtr(const AnyPtr &another)
      : _ptr(another._ptr), _handle(another._handle->clone()) {}

  AnyPtr &operator=(const AnyPtr &another) {
    if (&another == this) {
      return *this;
    }
    _ptr = another._ptr;
    delete _handle;
    _handle = another._handle;
    return *this;
  }

  core::Any get() { return _handle->get(_ptr); }

  void set(const core::Any &value) { return _handle->set(_ptr, value); }

  const std::type_info &type() const { return _handle->type(); }
};
}; // namespace firefly::core