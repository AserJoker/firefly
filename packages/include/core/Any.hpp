#pragma once
#include <cstddef>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>
namespace firefly::core {
struct AnyImplBase {
  std::string type;
  AnyImplBase(const std::string &name) : type(name) {}
  virtual ~AnyImplBase(){};
  virtual AnyImplBase *clone() = 0;
};
template <class T> struct AnyImpl : AnyImplBase {
  T value;
  AnyImpl(T value)
      : AnyImplBase(typeid(T).name()), value(std::forward<T>(value)) {}
  AnyImplBase *clone() override { return new AnyImpl<T>(value); }
};

struct AnyHandleBase {
  virtual bool equal(const AnyImplBase *a, const AnyImplBase *b) = 0;
  virtual AnyHandleBase *clone() = 0;
  virtual ~AnyHandleBase() {}
};

template <class T> struct AnyHandle : AnyHandleBase {
  bool equal(const AnyImplBase *a, const AnyImplBase *b) override {
    auto left = dynamic_cast<const AnyImpl<T> *>(a);
    auto right = dynamic_cast<const AnyImpl<T> *>(b);
    return left != nullptr && right != nullptr && left->value == right->value;
  }
  AnyHandleBase *clone() override { return new AnyHandle<T>(); }
};

struct Any {
  AnyImplBase *_base;
  AnyHandleBase *_handle;
  Any()
      : _base(new AnyImpl<std::nullptr_t>(nullptr)),
        _handle(new AnyHandle<std::nullptr_t>()) {}
  template <class T>
  Any(T value)
      : _base(new AnyImpl<std::remove_reference_t<std::remove_cv_t<T>>>(value)),
        _handle(new AnyHandle<std::remove_reference_t<std::remove_cv_t<T>>>) {}

  Any(const Any &another)
      : _base(another._base->clone()), _handle(another._handle->clone()) {}

  Any &operator=(const Any &another) {
    if (&another == this) {
      return *this;
    }
    delete _base;
    _base = another._base->clone();
    delete _handle;
    _handle = another._handle->clone();
    return *this;
  }

  ~Any() {
    if (_handle) {
      delete _handle;
      _handle = nullptr;
    }
    if (_base) {
      delete _base;
      _base = nullptr;
    }
  }

  template <class T> T &as() {
    if (_base->type != typeid(T).name()) {
      throw std::bad_cast();
    }
    return dynamic_cast<AnyImpl<T> *>(_base)->value;
  }

  template <class T> const T &as() const {
    if (_base->type != typeid(T).name()) {
      throw std::bad_cast();
    }
    return dynamic_cast<AnyImpl<T> *>(_base)->value;
  }

  template <class T> bool is() const { return _base->type == typeid(T).name(); }

  bool operator==(const Any &another) const {
    return _handle->equal(_base, another._base);
  }
  
  bool operator!=(const Any &another) const { return !(*this == another); }
};
}; // namespace firefly::core