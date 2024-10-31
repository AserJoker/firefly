#pragma once
#include "Array.hpp"
#include <cstddef>
#include <initializer_list>
#include <unordered_map>
#include <utility>
#include <variant>
namespace firefly::core {
template <class K, class T> class Map {
public:
  using KeyType = K;
  using ValueType = T;
  using Iterator = std::unordered_map<K, T>::iterator;
  using ConstIterator = std::unordered_map<K, T>::const_iterator;
  using Type = std::pair<K, T>;

private:
  std::unordered_map<K, T> _data;

public:
  Map() {}

  Map(const std::unordered_map<K, T> &data) : _data(data) {}

  Map(std::unordered_map<K, T> &&data) : _data(data) {}

  template <class Tv, class Kv>
  constexpr Map(const std::initializer_list<std::pair<Tv, Kv>> &initialize) {
    for (auto &[k, v] : initialize) {
      _data[k] = v;
    }
  }

  Map(const Map &another) : _data(another._data){};

  Map(Map &&another) : _data(std::move(another._data)){};

  Map<K, T> &operator=(const Map<K, T> &another) {
    _data = another._data;
    return *this;
  }

  Map<K, T> &operator=(Map<K, T> &&another) {
    _data = std::move(another._data);
    return *this;
  }

  virtual ~Map() {}

  core::Boolean_t operator==(const Map<K, T> &data) const {
    if (_data.size() != data.size()) {
      return false;
    }
    for (auto &[key, value] : _data) {
      if (!data.contains(key)) {
        return false;
      }
      if (value != data.at(key)) {
        return false;
      }
    }
    return true;
  }

  size_t size() const { return _data.size(); }

  core::Boolean_t empty() const { return _data.empty(); }

  Array<K> keys() const {
    Array<K> keys;
    keys.reserve(size());
    for (auto &[k, _] : _data) {
      keys.pushBack(k);
    }
    return keys;
  }

  void clear() { _data.clear(); }

  Iterator begin() { return _data.begin(); }

  Iterator end() { return _data.end(); }

  ConstIterator begin() const { return _data.begin(); }

  ConstIterator end() const { return _data.end(); }

  core::Boolean_t contains(const K &key) const { return _data.contains(key); }

  core::Boolean_t contains(K &&key) const {
    return _data.contains(std::forward<K>(key));
  }

  ConstIterator find(const K &key) const {
    for (auto it = begin(); it != end(); it++) {
      if (it->first == key) {
        return it;
      }
    }
    return end();
  }

  auto insert(const Type &value) { return _data.insert(value); }

  auto insert(Type &&value) { return _data.insert(std::forward<Type>(value)); }

  auto insert(const KeyType &key, const ValueType &value) {
    return _data.insert(key, value);
  }
  auto insert(KeyType &&key, ValueType &&value) {
    return _data.insert(std::forward<KeyType>(key),
                        std::forward<ValueType>(value));
  }

  auto insert(ConstIterator begin, ConstIterator end) {
    return _data.insert(begin, end);
  }

  T &at(const KeyType &key) {
    if (!contains(key)) {
      throw std::bad_variant_access();
    }
    return _data.at(key);
  }

  const T &at(const KeyType &key) const {
    if (!contains(key)) {
      throw std::bad_variant_access();
    }
    return _data.at(key);
  }

  T &at(const KeyType &&key) {
    if (!contains(key)) {
      throw std::bad_variant_access();
    }
    return _data.at(key);
  }

  const T &at(const KeyType &&key) const {
    if (!contains(key)) {
      throw std::bad_variant_access();
    }
    return _data.at(key);
  }

  T &operator[](const KeyType &key) { return _data[key]; }

  const T &operator[](const KeyType &key) const { return _data.at(key); }

  auto erase(const KeyType &key) { return _data.erase(key); }

  auto erase(ConstIterator position) { return _data.erase(position); }

  auto erase(ConstIterator begin, ConstIterator end) {
    return _data.erase(begin, end);
  }

  void erase(std::initializer_list<core::String_t> keys) {
    for (auto &key : keys) {
      erase(key);
    }
  }
};
} // namespace firefly::core