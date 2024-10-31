#pragma once
#include "Object.hpp"
#include "core/AutoPtr.hpp"
#include <mutex>
#include <shared_mutex>

namespace firefly::core {
template <class T> class Promise;

enum class PROMISE_STATUS { PENDDING, FULFILLED, REJECTED };

template <class T> class Promise : public Object {
private:
  PROMISE_STATUS _status;
  T _value{};
  core::String_t _messge;
  std::shared_mutex _mutex;

public:
  Promise() : _status(PROMISE_STATUS::PENDDING){};
  void resolve(const T &value) {
    std::unique_lock<std::shared_mutex> lock(_mutex);
    if (_status == PROMISE_STATUS::PENDDING) {
      _value = value;
      _status = PROMISE_STATUS::FULFILLED;
    }
  }

  void reject(const core::String_t &messge) {
    std::unique_lock<std::shared_mutex> lock(_mutex);
    if (_status == PROMISE_STATUS::PENDDING) {
      _messge = messge;
      _status = PROMISE_STATUS::REJECTED;
    }
  }
  const PROMISE_STATUS &getStatus() const {
    std::shared_lock<std::shared_mutex> lock(
        const_cast<std::shared_mutex &>(_mutex));
    return _status;
  }
  const T &getValue() const {
    std::shared_lock<std::shared_mutex> lock(
        const_cast<std::shared_mutex &>(_mutex));
    return _value;
  }
  const core::String_t &getMessage() const {
    std::shared_lock<std::shared_mutex> lock(
        const_cast<std::shared_mutex &>(_mutex));
    return _messge;
  }
};
template <class T> using Promisify = AutoPtr<Promise<T>>;
}; // namespace firefly::core