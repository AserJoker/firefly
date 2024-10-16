#pragma once
#include <algorithm>
#include <initializer_list>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include <variant>
#include <vector>
namespace firefly::core {
template <class T> class Array {
public:
  using Type = T;
  using Iterator = std::vector<T>::iterator;
  using ConstIterator = std::vector<T>::const_iterator;
  using ReverseIterator = std::vector<T>::reverse_iterator;
  using ConstReverseIterator = std::vector<T>::const_reverse_iterator;

protected:
  std::vector<T> _data;

public:
  Array() {}

  Array(std::initializer_list<T> data) : _data(data) {}

  Array(const Array<T> &another) { _data = another._data; }

  Array(Array<T> &&another) { _data = std::move(another._data); }

  Array<T> &operator=(const std::vector<T> &data) {
    _data = data;
    return *this;
  }

  Array<T> &operator=(std::vector<T> &&data) {
    _data = std::move(data);
    return *this;
  }

  Array<T> &operator=(const Array<T> &another) {
    _data = another._data;
    return *this;
  }

  Array<T> &operator=(Array<T> &&another) {
    _data = std::move(another._data);
    return *this;
  }

  bool operator==(const std::vector<T> &data) const {
    if (_data.size() != data.size()) {
      return false;
    }
    for (size_t index = 0; index < size(); index++) {
      if (_data[index] != data[index]) {
        return false;
      }
    }
    return true;
  }

  bool operator==(const Array<T> &another) const {
    auto &data = another._data;
    if (_data.size() != data.size()) {
      return false;
    }
    for (size_t index = 0; index < size(); index++) {
      if (_data[index] != data[index]) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const std::vector<T> &data) const { return !(*this == data); }
  bool operator!=(const Array<T> &data) const { return !(*this == data); }

  Array<T> &operator+=(const T &data) {
    pushBack(data);
    return *this;
  }

  Array<T> &operator-=(const T &data) {
    erase(data);
    return *this;
  }

  void pushBack(T &&item) { _data.push_back(item); }

  void pushBack(std::initializer_list<T> items) {
    _data.reserve(items.size());
    for (auto &item : items) {
      pushBack(std::move(item));
    }
  }

  void pushBack(const T &item) { _data.push_back(item); }

  void popBack() { _data.pop_back(); }

  size_t size() const { return _data.size(); }

  bool empty() const { return _data.empty(); }

  void resize(size_t size) { _data.resize(size); }

  void reserve(size_t size) { _data.reserve(size); }

  void clear() { _data.clear(); }

  T &operator[](size_t index) { return at(index); }

  const T &operator[](size_t index) const { return at(index); }

  T &at(size_t index) {
    if (index >= _data.size()) {
      throw std::bad_variant_access();
    }
    return _data.at(index);
  }

  const T &at(size_t index) const {
    if (index >= _data.size()) {
      throw std::bad_variant_access();
    }
    return _data.at(index);
  }

  size_t indexOf(const T &data) const {
    auto it = find(data);
    return it - begin();
  }

  T *data() { return _data.data(); }

  const T *data() const { return _data.data(); }

  operator std::vector<T>() { return _data; }

  Iterator begin() { return _data.begin(); }

  Iterator end() { return _data.end(); }

  ConstIterator begin() const { return _data.begin(); }

  ConstIterator end() const { return _data.end(); }

  ReverseIterator rbegin() { return _data.rbegin(); }

  ReverseIterator rend() { return _data.rend(); }

  ConstReverseIterator rbegin() const { return _data.rbegin(); }

  ConstReverseIterator rend() const { return _data.rend(); }

  ConstIterator find(T &&data) const {
    return std::find(_data.begin(), _data.end(), data);
  }
  ConstIterator find(const T &data) const {
    return std::find(_data.begin(), _data.end(), data);
  }

  ReverseIterator rfind(const T &data) {
    return std::find(_data.rbegin(), _data.rend(), data);
  }

  ConstReverseIterator rfind(const T &data) const {
    return std::find(_data.rbegin(), _data.rend(), data);
  }

  bool contains(const T &data) const {
    return std::find(_data.begin(), _data.end(), data) != _data.end();
  }

  Iterator insert(ConstIterator position, T &&data) {
    return _data.insert(position, std::forward<T>(data));
  }

  Iterator insert(ConstIterator position, const T &data) {
    return _data.insert(position, data);
  }

  Iterator insert(ConstIterator position, std::initializer_list<T> data) {
    return _data.insert(position, data.begin(), data.end());
  }

  Iterator insert(ConstIterator position, auto &&...items) {
    return insert(position, {std::forward<T>(items)...});
  }

  Iterator erase(ConstIterator iterator) { return _data.erase(iterator); }

  Iterator erase(ConstIterator begin, ConstIterator end) {
    return _data.erase(begin, end);
  }

  ConstIterator erase(const T &data, bool removeAll = false) {
    auto it = find(data);
    if (it == end()) {
      return it;
    }
    if (removeAll) {
      Iterator next;
      while (it != end()) {
        next = erase(it);
        it = find(data);
      }
      _data.shrink_to_fit();
      return next;
    } else {
      return erase(it);
    }
  }

  Array<T> reverseCopy() const {
    Array<T> result;
    result._data.reserve(size());
    return std::reverse_copy(begin(), end(), result.begin());
  }

  void reverse() { std::reverse(begin(), end()); }

  template <typename Compare> Array<T> sortCopy(Compare fn) const {
    Array<T> result = *this;
    result.sort(fn);
    return result;
  }

  template <typename Compare> void sort(Compare fn) {
    std::sort(begin(), end(), fn);
  }

  Array<T> &replace(const T &source, T &&data) {
    auto index = indexOf(source);
    if (index < size()) {
      _data[index] = data;
    }
    return *this;
  }

  Array<T> &replace(const T &source, const T &data) {
    auto index = indexOf(source);
    if (index < size()) {
      _data[index] = data;
    }
    return *this;
  }

  Array<T> &replaceAll(const T &source, T &&data) {
    auto index = indexOf(source);
    while (index < size()) {
      _data[index] = data;
      index = indexOf(source);
    }
    return *this;
  }

  Array<T> &replaceAll(const T &source, const T &data) {
    auto index = indexOf(source);
    while (index < size()) {
      _data[index] = data;
      index = indexOf(source);
    }
    return *this;
  }

  const std::string toString() const {
    std::string result = "[";
    for (size_t index = 0; index < size(); index++) {
      auto &item = at(index);
      std::stringstream ss;
      ss << item;
      result.append(ss.str());
      if (index != size() - 1) {
        result.append(",");
      }
    }
    result.append("]");
    return result;
  }
};

template <class T>
constexpr inline std::ostream &operator<<(std::ostream &out,
                                          const Array<T> &array) {
  out << array.toString();
  return out;
}
}; // namespace firefly::core