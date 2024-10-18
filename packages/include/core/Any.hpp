#pragma once
#include <string>
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
struct Any {
  AnyImplBase *_base;

  template <class T> Any(T value) : _base(new AnyImpl<T>(value)) {}

  Any(const Any &another) : _base(another._base->clone()) {}

  Any &operator=(const Any &another) {
    delete _base;
    _base = another._base->clone();
    return *this;
  }

  ~Any() {
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
};
}; // namespace firefly::core