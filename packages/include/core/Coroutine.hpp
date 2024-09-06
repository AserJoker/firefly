#pragma once
#include "AutoPtr.hpp"
#include "Promise.hpp"
#include "core/Object.hpp"
#include "exception/Exception.hpp"
#include <functional>
namespace firefly::core {
class Coroutine : public Object {
public:
  static void init();
  static void start(const std::function<void()> &func);
  static void yield();
  static bool done();
  template <class T>
  static AutoPtr<Promise<T>> async(const std::function<T()> &func) {
    AutoPtr pro = new Promise<T>();
    start([=]() -> void {
      auto promise = pro;
      promise->resolve(func());
    });
    return pro;
  };
  template <class T> static const T &wait(core::AutoPtr<Promise<T>> promise) {
    while (promise->template getStatus() == PROMISE_STATUS::PENDDING) {
      yield();
    }
    if (promise->getStatus() == PROMISE_STATUS::FULFILLED) {
      return promise->getValue();
    }
    throw exception::RuntimeException<"Promise Reject">(promise->getMessage());
  }
  template <class T>
  static void next(core::AutoPtr<Promise<T>> promise,
                   const std::function<void(const T &)> &callback) {
    start([=]() -> void { callback(wait(promise)); });
  }
};
} // namespace firefly::core