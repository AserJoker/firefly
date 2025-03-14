#pragma once
#include <cmath>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

template <class T = uint8_t> class BigInt {
private:
  std::vector<T> _data{};
  bool _negative{};

public:
  BigInt() : _data({0}), _negative(false) {}

  BigInt(const BigInt &another)
      : _data(another._data), _negative(another._negative) {}

  BigInt(BigInt &&another)
      : _data(another._data), _negative(another._negative) {}

  BigInt(int64_t number) : _negative(number < 0) {
    static uint64_t max = (uint64_t)((T)-1) + 1;
    if (number < 0) {
      number = -number;
    }
    while (number > 0) {
      _data.push_back((T)(number % max));
      number >>= (sizeof(T) * 8);
    }
    if (_data.empty()) {
      _data.push_back(0);
    }
  }

  BigInt(const std::wstring &source) : _data({0}), _negative(false) {
    auto chr = source.c_str();
    if (*chr == L'+') {
      chr++;
    } else if (*chr == L'-') {
      _negative = true;
      chr++;
    }
    while (*chr) {
      if (*chr < '0' || *chr > '9') {
        throw std::runtime_error("Cannot convert to a BigInt");
      }
      *this *= 10;
      *this += (uint64_t)(*chr - '0');
      chr++;
    }
    while (*_data.rbegin() == 0) {
      _data.pop_back();
    }
    if (_data.empty()) {
      _data.push_back(0);
    }
  }

  std::wstring toString() const {
    std::wstring result;
    BigInt<> tmp = *this;
    tmp._negative = 0;
    if (tmp == 0) {
      result += L"0";
    } else {
      while (tmp > 0) {
        result += (tmp % 10)._data[0] + '0';
        tmp /= 10;
      }
    }
    result = std::wstring(result.rbegin(), result.rend());
    if (_negative) {
      result = L"-" + result;
    }
    return result;
  }

  std::optional<int64_t> toInt64() const {
    int64_t result = 0;
    uint32_t step = 0;
    if (_data.size() == 1) {
      return _data[0];
    }
    for (auto &item : _data) {
      int64_t tmp = item;
      tmp <<= step;
      step += sizeof(T) * 8;
      if (step > 63) {
        return std::nullopt;
      }
      result |= tmp;
    }
    if (_negative) {
      result = -result;
    }
    return result;
  }

  BigInt abs() const {
    BigInt result = *this;
    result._negative = true;
    return result;
  }

  BigInt uadd(const BigInt &another) const {
    static uint64_t max = (uint64_t)((T)-1) + 1;
    size_t index = 0;
    uint64_t next = 0;
    BigInt result;
    while (next != 0 || _data.size() > index || another._data.size() > index) {
      uint64_t val = next;
      if (index < _data.size()) {
        val += _data[index];
      }
      if (index < another._data.size()) {
        val += another._data[index];
      }
      if (index >= result._data.size()) {
        result._data.push_back(0);
      }
      result._data[index] = (T)(val % max);
      next = val / max;
      index++;
    }
    return result;
  }

  BigInt usub(const BigInt &another) const {
    BigInt left = abs();
    BigInt right = another.abs();
    BigInt *a = &left;
    BigInt *b = &right;
    bool neg = false;
    if (left > right) {
      a = &right;
      b = &left;
      neg = true;
    }
    int64_t next = 0;
    BigInt result;
    result._data.clear();
    for (size_t index = 0; index < a->_data.size(); index++) {
      uint64_t pa = a->_data[index];
      pa += next;
      if (index < b->_data.size()) {
        uint64_t pb = b->_data[index];
        if (pa < pb) {
          pa += 1 << sizeof(T) * 8;
          next = -1;
        }
        result._data.push_back((T)(pa - pb));
      } else {
        result._data.push_back((T)pa);
      }
    }
    while (result._data.size() > 1 && *result._data.rbegin() == 0) {
      result._data.pop_back();
    }
    result._negative = neg;
    return result;
  }

  bool isNetative() const { return _negative; }

  BigInt &operator=(const BigInt &another) {
    if (this == &another) {
      return *this;
    }
    _negative = another._negative;
    _data = another._data;
    return *this;
  }

  BigInt operator+(const BigInt &another) const {
    if (_negative == another._negative) {
      BigInt result = uadd(another);
      result._negative = _negative;
      return result;
    } else {
      return *this - (-another);
    }
  }

  BigInt operator*(const BigInt &another) const {

    static uint64_t max = (uint64_t)((T)-1) + 1;
    BigInt result;
    uint64_t rindex = 0;
    for (auto &part : another._data) {
      uint64_t next = 0;
      for (size_t index = 0; index < _data.size(); index++) {
        auto &src = _data[index];
        if (index + rindex >= result._data.size()) {
          result._data.push_back(0);
        }
        auto val = part * src + next;
        result._data[index + rindex] += (T)(val % max);
        next = val / max;
      }
      if (next > 0) {
        result._data.push_back((T)(next));
      }
      rindex++;
    }
    while (result._data.size() > 1 && *result._data.rbegin() == 0) {
      result._data.pop_back();
    }
    result._negative = _negative != another._negative;
    return result;
  }

  BigInt operator-(const BigInt &another) const {
    if (_negative == another._negative) {
      BigInt result = usub(another);
      if (_negative) {
        result._negative = !result._negative;
      }
      return result;
    } else {
      return *this + (-another);
    }
  }

  BigInt operator/(const BigInt &another) const {
    if (another == 0) {
      throw std::runtime_error("Division by zero");
    }
    if (another > *this) {
      return 0;
    }
    if (another == *this) {
      return 1;
    }
    size_t len = _data.size() - another._data.size();
    BigInt next;
    BigInt result;
    std::vector<T> rdata;
    for (size_t index = 0; index <= len; index++) {
      BigInt tmp;
      tmp._data.clear();
      std::vector<T> data;
      for (size_t i = 0; i < another._data.size(); i++) {
        data.push_back(_data[_data.size() - 1 - index - i]);
      }
      for (auto it = data.rbegin(); it != data.rend(); it++) {
        tmp._data.push_back(*it);
      }
      tmp += next * (int64_t)std::pow(2, sizeof(T) * 8);
      T v = 0;
      while (tmp >= another) {
        tmp -= another;
        v++;
      }
      next = tmp;
      rdata.push_back(v);
    }
    result._data.clear();
    while (rdata.size() > 1 && *rdata.begin() == 0) {
      rdata.erase(rdata.begin());
    }
    for (auto it = rdata.rbegin(); it != rdata.rend(); it++) {
      result._data.push_back(*it);
    }
    return result;
  }

  BigInt operator%(const BigInt &another) const {
    if (another == 0) {
      throw std::runtime_error("Division by zero");
    }
    if (another > *this) {
      return *this;
    }
    if (another == *this) {
      return 0;
    }
    size_t len = _data.size() - another._data.size();
    BigInt next;
    for (size_t index = 0; index <= len; index++) {
      BigInt tmp;
      tmp._data.clear();
      std::vector<T> data;
      for (size_t i = 0; i < another._data.size(); i++) {
        data.push_back(_data[_data.size() - 1 - index - i]);
      }
      for (auto it = data.rbegin(); it != data.rend(); it++) {
        tmp._data.push_back(*it);
      }
      tmp += next * (int64_t)(std::pow(2, sizeof(T) * 8));
      while (tmp >= another) {
        tmp -= another;
      }
      next = tmp;
    }
    next._negative = _negative;
    return next;
  }

  BigInt operator+() const { return *this; }

  BigInt operator-() const {
    BigInt result = *this;
    result._negative = !result._negative;
    return result;
  }

  BigInt operator~() const {
    BigInt result = *this;
    result += 1;
    result = -result;
    return result;
  }

  bool operator>(const BigInt &another) const {

    if (!_negative && another._negative) {
      return true;
    } else if (_negative && !another._negative) {
      return false;
    }
    if (_data.size() > another._data.size()) {
      return !_negative;
    } else if (_data.size() < another._data.size()) {
      return _negative;
    }
    for (size_t index = 0; index < _data.size(); index++) {
      if (_data[index] > another._data[index]) {
        return !_negative;
      } else if (_data[index] < another._data[index]) {
        return _negative;
      }
    }
    return false;
  }

  bool operator<(const BigInt &another) const {
    if (!_negative && another._negative) {
      return false;
    } else if (_negative && !another._negative) {
      return true;
    }
    if (_data.size() > another._data.size()) {
      return _negative;
    } else if (another._data.size() > _data.size()) {
      return !_negative;
    }
    for (size_t index = 0; index < _data.size(); index++) {
      if (_data[index] > another._data[index]) {
        return _negative;
      } else if (_data[index] < another._data[index]) {
        return !_negative;
      }
    }
    return false;
  }

  bool operator>=(const BigInt &another) const {
    if (!_negative && another._negative) {
      return true;
    } else if (_negative && !another._negative) {
      return false;
    }
    if (_data.size() > another._data.size()) {
      return !_negative;
    } else if (_data.size() < another._data.size()) {
      return _negative;
    }
    for (size_t index = 0; index < _data.size(); index++) {
      if (_data[index] > another._data[index]) {
        return !_negative;
      } else if (_data[index] < another._data[index]) {
        return _negative;
      }
    }
    return true;
  }

  bool operator<=(const BigInt &another) const {
    if (!_negative && another._negative) {
      return false;
    } else if (_negative && !another._negative) {
      return true;
    }
    if (_data.size() > another._data.size()) {
      return _negative;
    } else if (_data.size() < another._data.size()) {
      return !_negative;
    }
    for (size_t index = 0; index < _data.size(); index++) {
      if (_data[index] > another._data[index]) {
        return _negative;
      } else if (_data[index] < another._data[index]) {
        return !_negative;
      }
    }
    return true;
  }

  bool operator==(const BigInt &another) const {
    if (_negative != another._negative) {
      return false;
    }
    if (_data.size() != another._data.size()) {
      return false;
    }
    for (size_t index = 0; index < _data.size(); index++) {
      if (_data[index] != another._data[index]) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const BigInt &another) const {
    if (_negative != another._negative) {
      return true;
    }
    if (_data.size() != another._data.size()) {
      return true;
    }
    for (size_t index = 0; index < _data.size(); index++) {
      if (_data[index] != another._data[index]) {
        return true;
      }
    }
    return false;
  }

  BigInt pow(const BigInt &another) const {
    BigInt result = 1;
    for (BigInt i = 0; i < another; i += 1) {
      result *= *this;
    }
    return result;
  }

  BigInt operator<<(const BigInt &another) const {
    BigInt result = *this;
    for (BigInt i = 0; i < another; i += 1) {
      result *= 2;
    }
    return result;
  }

  BigInt operator>>(const BigInt &another) const {
    BigInt result = *this;
    for (BigInt i = 0; i < another; i += 1) {
      result /= 2;
    }
    return result;
  }

  BigInt operator&(const BigInt &another) const {
    BigInt result;
    result._data.clear();
    size_t index = 0;
    BigInt a = *this;
    BigInt b = another;
    BigInt max;
    max._data.clear();
    while (a._data.size() > index || b._data.size() > index) {
      max._data.push_back((T)-1);
      index++;
    }
    if (a._negative) {
      a = max + 1 + a;
    }
    if (b._negative) {
      b = max + 1 + b;
    }
    index = 0;
    while (a._data.size() > index || b._data.size() > index) {
      T val1 = 0;
      T val2 = 0;
      if (index < a._data.size()) {
        val1 = a._data[index];
      }
      if (index < b._data.size()) {
        val2 = b._data[index];
      }
      result._data.push_back(val1 & val2);
      index++;
    }
    while (!*result._data.rbegin()) {
      result._data.pop_back();
    }
    if (result._data.empty()) {
      result._data.push_back(0);
    }
    if (_negative && another._negative) {
      result = max + 1 - result;
    }
    result._negative = _negative && another._negative;
    return result;
  }

  BigInt operator|(const BigInt &another) const {
    BigInt result;
    result._data.clear();
    size_t index = 0;
    BigInt a = *this;
    BigInt b = another;
    BigInt max;
    max._data.clear();
    while (a._data.size() > index || b._data.size() > index) {
      max._data.push_back((T)-1);
      index++;
    }
    if (a._negative) {
      a = max + 1 + a;
    }
    if (b._negative) {
      b = max + 1 + b;
    }
    index = 0;
    while (a._data.size() > index || b._data.size() > index) {
      T val1 = 0;
      T val2 = 0;
      if (index < a._data.size()) {
        val1 = a._data[index];
      }
      if (index < b._data.size()) {
        val2 = b._data[index];
      }
      result._data.push_back(val1 | val2);
      index++;
    }
    while (!*result._data.rbegin()) {
      result._data.pop_back();
    }
    if (result._data.empty()) {
      result._data.push_back(0);
    }
    if (_negative || another._negative) {
      result = max + 1 - result;
    }
    result._negative = _negative || another._negative;
    return result;
  }

  BigInt operator^(const BigInt &another) const {
    return (*this & ~another) | (~*this & another);
  }

  BigInt &operator*=(const BigInt &another) { return *this = *this * another; }

  BigInt &operator+=(const BigInt &another) { return *this = *this + another; }

  BigInt &operator-=(const BigInt &another) { return *this = *this - another; }

  BigInt &operator/=(const BigInt &another) { return *this = *this / another; }

  BigInt &operator%=(const BigInt &another) { return *this = *this % another; }
};