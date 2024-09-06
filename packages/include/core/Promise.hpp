#pragma once
#include "Object.hpp"
namespace firefly::core {
template <class T> class Promise;

enum class PROMISE_STATUS { PENDDING, FULFILLED, REJECTED };

template <class T> class Promise : public Object {
public:
private:
  PROMISE_STATUS _status;
  T _value{};
  std::string _messge;

public:
  Promise() : _status(PROMISE_STATUS::PENDDING){};
  void resolve(const T &value) {
    if (_status == PROMISE_STATUS::PENDDING) {
      _value = value;
      _status = PROMISE_STATUS::FULFILLED;
    }
  }

  void reject(const std::string &messge) {
    if (_status == PROMISE_STATUS::PENDDING) {
      _messge = messge;
      _status = PROMISE_STATUS::REJECTED;
    }
  }
  const PROMISE_STATUS &getStatus() const { return _status; }
  const T &getValue() const { return _value; }
  const std::string &getMessage() const { return _messge; }
};
}; // namespace firefly::core