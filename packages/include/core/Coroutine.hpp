#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <any>
#include <cstring>
#include <exception>
#include <functional>
#ifdef __unix__
#include <ucontext.h>
#else
#include <windows.h>
#endif
namespace firefly::core {
enum class CoPromiseType { FULFILLED, REJECTED, PENDDING };
template <class T> class CoPromise : public Object {
private:
  std::exception _error;
  T _value;
  CoPromiseType _status;

public:
  CoPromise() { _status = CoPromiseType::PENDDING; }
  void resolve(T value) {
    if (_status == CoPromiseType::PENDDING) {
      _value = value;
      _status = CoPromiseType::FULFILLED;
    }
  }
  void reject(const std::exception &e) {
    if (_status == CoPromiseType::PENDDING) {
      _error = e;
      _status = CoPromiseType::REJECTED;
    }
  }
  const CoPromiseType &status() const { return _status; }
  const T &value() const { return _value; }
  const std::exception &error() const { return _error; }
};

template <> class CoPromise<void> : public Object {
private:
  std::exception _error;
  CoPromiseType _status;

public:
  CoPromise() { _status = CoPromiseType::PENDDING; }
  void resolve() {
    if (_status == CoPromiseType::PENDDING) {
      _status = CoPromiseType::FULFILLED;
    }
  }
  void reject(const std::exception &e) {
    if (_status == CoPromiseType::PENDDING) {
      _error = e;
      _status = CoPromiseType::REJECTED;
    }
  }
  const CoPromiseType &status() const { return _status; }
  const std::exception &error() const { return _error; }
};
class CoContext {
public:
  inline static constexpr size_t STACK_SIZE = 4096;

public:
private:
  template <class T = std::any> class Coroutine {
  private:
    friend class CoContext;

  private:
    uint8_t stack[STACK_SIZE];
    Coroutine<void> *next;
    std::function<void()> exec;
#ifdef __unix__
    ucontext_t cpuinfo;
#else
    LPVOID fiber;
#endif

    AutoPtr<CoPromise<T>> result;
    bool running;

  public:
    Coroutine() {
      next = nullptr;
      exec = nullptr;
      running = true;
      result = new CoPromise<T>();
      memset(stack, 0, STACK_SIZE);
#ifdef __unix__
      getcontext(&cpuinfo);
#endif
    }
#ifndef __unix__
    ~Coroutine() {
      if (fiber != GetCurrentFiber()) {
        DeleteFiber(fiber);
      }
    }
#endif
  };

  inline static Coroutine<void> *current = nullptr;

  static void schedule() {
    try {
      current->exec();
    } catch (std::exception &e) {
      current->result->reject(e);
    }
    current->running = false;
    yield();
  }

public:
  template <class T, class... ARGS, class... VARGS>
  static AutoPtr<CoPromise<T>> start(const std::function<T(ARGS...)> &exec,
                                     VARGS... args) {
    Coroutine<T> *routine = new Coroutine<T>;
    routine->exec = [=]() -> void { routine->result->resolve(exec(args...)); };
#ifdef __unix__
    routine->cpuinfo.uc_stack.ss_sp = routine->stack;
    routine->cpuinfo.uc_stack.ss_size = STACK_SIZE;
    routine->cpuinfo.uc_stack.ss_flags = 0;
    makecontext(&routine->cpuinfo, schedule, 0);
#else
    routine->fiber =
        CreateFiber(STACK_SIZE, (LPFIBER_START_ROUTINE)schedule, NULL);
#endif
    routine->next = current->next;
    current->next = (Coroutine<void> *)routine;
    return routine->result;
  }
  template <class... ARGS, class... VARGS>
  static AutoPtr<CoPromise<void>>
  start(const std::function<void(ARGS...)> &exec, VARGS... args) {
    Coroutine<void> *routine = new Coroutine<void>;
    routine->exec = [=]() -> void {
      exec(args...);
      routine->result->resolve();
    };
#ifdef __unix__
    routine->cpuinfo.uc_stack.ss_sp = routine->stack;
    routine->cpuinfo.uc_stack.ss_size = 4096;
    routine->cpuinfo.uc_stack.ss_flags = 0;
    makecontext(&routine->cpuinfo, schedule, 0);
#else
    routine->fiber =
        CreateFiber(STACK_SIZE, (LPFIBER_START_ROUTINE)schedule, NULL);
#endif
    routine->next = current->next;
    current->next = routine;
    return routine->result;
  }
  static void yield() {
    auto *old = current;
    auto *last = old;
    current = current->next;
    while (!current->running) {
      auto *tmp = current;
      current = current->next;
      delete tmp;
      last->next = current;
    }
    if (current != old) {
#ifdef __unix__
      swapcontext(&old->cpuinfo, &current->cpuinfo);
#else
      SwitchToFiber(current->fiber);
#endif
    }
  }

  static void init() {
    current = new Coroutine<void>;
    current->next = current;
#ifndef __unix__
    current->fiber = ConvertThreadToFiber(NULL);
#endif
  }
  static void dispose() {
    while (current->next != current) {
      auto p = current->next;
      current->next = current->next->next;
      delete p;
    }
    delete current;
    current = nullptr;
  }

  static bool ready() { return current->next == current; }

  template <class T> static T wait(const AutoPtr<CoPromise<T>> promise) {
    while (promise->status() == CoPromiseType::PENDDING) {
      yield();
    }
    if (promise->status() == CoPromiseType::FULFILLED) {
      return promise->value();
    } else {
      throw promise->error();
    }
  }
};
}; // namespace firefly::core