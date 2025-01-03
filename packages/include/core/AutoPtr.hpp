#pragma once

#include "NullpointerException.hpp"

namespace firefly::core {
template <class T> class AutoPtr {
public:
  using innerType = T;

private:
  T *_object;

  void dispose() {
    if (_object) {
      if (!_object->subRef()) {
        delete _object;
        _object = nullptr;
      }
    }
  }

public:
  AutoPtr(T *object = nullptr) : _object(object) {
    if (_object) {
      _object->addRef();
    }
  }

  virtual ~AutoPtr() { dispose(); }

  AutoPtr(const AutoPtr<T> &another) : _object(nullptr) {
    if (another != nullptr) {
      _object = const_cast<T *>(&(*another));
    }
    if (_object) {
      _object->addRef();
    }
  }
  const T *getRawPointer() const { return _object; }
  template <class K> AutoPtr(const AutoPtr<K> &another) : _object(nullptr) {
    if (another != nullptr) {
      _object = const_cast<K *>(another.getRawPointer());
    }
    if (_object) {
      _object->addRef();
    }
  }

  T &operator*() {
    if (!_object) {
      throw NullpointerException();
    }
    return *_object;
  }

  T *operator->() {
    if (!_object) {
      throw NullpointerException();
    }
    return _object;
  }

  const T &operator*() const {
    if (!_object) {
      throw NullpointerException();
    }
    return *_object;
  }

  const T *operator->() const {
    if (!_object) {
      throw NullpointerException();
    }
    return _object;
  }

  template <class K> AutoPtr<T> &operator=(K *object) {
    if (_object && _object != object) {
      dispose();
    }
    _object = object;
    if (_object) {
      _object->addRef();
    }
    return *this;
  }

  template <class K> AutoPtr<T> &operator=(const AutoPtr<K> &another) {
    if (_object && _object != another.getRawPointer()) {
      dispose();
    }
    _object = const_cast<K *>(another.getRawPointer());
    if (_object) {
      _object->addRef();
    }
    return *this;
  }

  AutoPtr<T> &operator=(const AutoPtr<T> &another) {
    if (this == &another) {
      return *this;
    }
    if (_object && _object != another.getRawPointer()) {
      dispose();
    }
    _object = (T *)another.getRawPointer();
    if (_object) {
      _object->addRef();
    }
    return *this;
  }

  template <class K> core::Boolean_t operator==(const K *object) const {
    return _object == object;
  }

  template <class K> core::Boolean_t operator!=(const K *object) const {
    return _object != object;
  }

  core::Boolean_t operator==(core::Nil_t) const { return _object == nullptr; }

  core::Boolean_t operator!=(core::Nil_t) const { return _object != nullptr; }

  template <class K>
  core::Boolean_t operator==(const AutoPtr<K> &another) const {
    return _object == another.getRawPointer();
  }

  template <class K>
  core::Boolean_t operator!=(const AutoPtr<K> &another) const {
    return _object != another.getRawPointer();
  }

  template <class K> AutoPtr<K> cast() { return dynamic_cast<K *>(_object); }
  template <class K> const AutoPtr<K> cast() const {
    return dynamic_cast<K *>(_object);
  }

  const core::Boolean_t operator!() const { return _object == nullptr; }
};
} // namespace firefly::core