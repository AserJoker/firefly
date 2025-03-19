#pragma once
#include "Ref.hpp"
#include <stdexcept>
namespace firefly::core {
template <class T> class AutoPtr {
private:
  const T *_instance;

public:
  AutoPtr(const T *instance = nullptr) : _instance(instance) {
    if (_instance) {
      Ref::addRef(_instance);
    }
  }

  AutoPtr(const AutoPtr<T> &another) : _instance(another._instance) {
    if (_instance) {
      Ref::addRef(_instance);
    }
  }

  template <class K>
  AutoPtr(const AutoPtr<K> &another) : _instance((const T *)another.raw()) {
    if (_instance) {
      Ref::addRef(_instance);
    }
  }

  virtual ~AutoPtr() {
    if (!Ref::dispose(_instance)) {
      delete _instance;
      _instance = nullptr;
    }
  }

  T *operator->() {
    if (!_instance) {
      throw std::runtime_error("Nullpointer exception");
    }
    return (T *)_instance;
  }

  T &operator*() {
    if (!_instance) {
      throw std::runtime_error("Nullpointer exception");
    }
    return *(T *)_instance;
  }

  T *operator->() const {
    if (!_instance) {
      throw std::runtime_error("Nullpointer exception");
    }
    return (T *)_instance;
  }

  T &operator*() const {
    if (!_instance) {
      throw std::runtime_error("Nullpointer exception");
    }
    return *(T *)_instance;
  }

  AutoPtr<T> &operator=(const T *object) {
    if (_instance == object) {
      return *this;
    }
    if (_instance) {
      if (!Ref::dispose(_instance)) {
        delete (T *)_instance;
        _instance = nullptr;
      }
    }
    _instance = object;
    if (_instance) {
      Ref::addRef(_instance);
    }
    return *this;
  }

  AutoPtr<T> &operator=(const AutoPtr<T> &another) {
    if (_instance == another._instance) {
      return *this;
    }
    if (_instance) {
      if (Ref::dispose(_instance)) {
        delete (T *)_instance;
        _instance = nullptr;
      }
    }
    _instance = another._instance;
    if (_instance) {
      Ref::addRef(_instance);
    }
    return *this;
  }
  template <class K> AutoPtr<T> &operator=(const AutoPtr<K> &another) {
    if (_instance == another.raw()) {
      return *this;
    }
    if (_instance) {
      if (Ref::dispose(_instance)) {
        delete (T *)_instance;
        _instance = nullptr;
      }
    }
    _instance = another.raw();
    if (_instance) {
      Ref::addRef(_instance);
    }
    return *this;
  }

  bool operator==(const T *another) const { return _instance == another; }

  bool operator==(const AutoPtr<T> &another) const {
    return _instance == another._instance;
  }

  template <class K> bool operator==(const AutoPtr<K> &another) const {
    return _instance == another.raw();
  }

  T *raw() const { return (T *)_instance; }

  template <class K> AutoPtr<K> cast() const {
    return dynamic_cast<K *>((T *)_instance);
  }

  bool operator!() const { return _instance == nullptr; }

  operator bool() const { return _instance != nullptr; }
};
template <class T> using AutoConstPtr = AutoPtr<const T>;
}; // namespace firefly::core