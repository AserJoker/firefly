#pragma once
#include "AutoPtr.hpp"
#include "Promise.hpp"
#include "core/Object.hpp"
#include "exception/Exception.hpp"
#include <functional>
namespace firefly::core {
class Coroutine : public Object {

public:
  static void start(const std::function<void()> &func);
  static void init();
  static void yield();
  static bool done();
  template <class T, class Fn> static AutoPtr<Promise<T>> async(Fn &&func) {
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
  template <class T, class Fn, class CFn>
  static void next(core::AutoPtr<Promise<T>> promise, Fn &&callback,
                   CFn &&catchCallback) {
    start([=]() -> void {
      try {
        callback(wait(promise));
      } catch (const std::exception &e) {
        catchCallback(e.what());
      }
    });
  }
};
} // namespace firefly::core