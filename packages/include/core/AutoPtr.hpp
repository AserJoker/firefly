#pragma once
#include <cstddef>
namespace firefly::core {
template <class T> class AutoPtr {
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
  AutoPtr(const AutoPtr<T> &another) : _object(const_cast<T *>(&*another)) {
    if (_object) {
      _object->addRef();
    }
  }
  template <class K>
  AutoPtr(const AutoPtr<K> &another) : _object(const_cast<K *>(&*another)) {
    if (_object) {
      _object->addRef();
    }
  }

  T &operator*() { return *_object; }
  T *operator->() { return _object; }

  const T &operator*() const { return *_object; }
  const T *operator->() const { return _object; }

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
    if (_object && _object != *another) {
      dispose();
    }
    _object = *another;
    if (_object) {
      _object->addRef();
    }
    return *this;
  }
  template <class K> bool operator==(const K *object) const {
    return _object == object;
  }
  template <class K> bool operator!=(const K *object) const {
    return _object != object;
  }
  bool operator==(std::nullptr_t) const { return _object == nullptr; }
  bool operator!=(std::nullptr_t) const { return _object != nullptr; }
  template <class K> bool operator==(const AutoPtr<K> &another) const {
    return _object == &*another;
  }
  template <class K> bool operator!=(const AutoPtr<K> &another) const {
    return _object != &*another;
  }
  template <class K> AutoPtr<K> cast() { return dynamic_cast<K *>(_object); }
};
} // namespace firefly::core