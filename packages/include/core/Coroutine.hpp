#pragma once
#include "AutoPtr.hpp"
#include "Promise.hpp"
#include "core/Object.hpp"
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
};
} // namespace firefly::core