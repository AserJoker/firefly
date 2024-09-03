#pragma once
#include "Object.hpp"
#include "core/AutoPtr.hpp"
#include <exception>
#include <functional>
#include <stdexcept>
#include <vector>
namespace firefly::core {
template <class T> class Promise;

enum class PROMISE_STATUS { PENDDING, FULFILLED, REJECTED, CANCELED };

template <> class Promise<void> : public Object {
  using Resolve = std::function<void()>;
  using Reject = std::function<void(const std::string &e)>;
  using Next = std::function<void()>;
  using Error = std::function<void(const std::string &e)>;
  using Finally = std::function<void()>;

private:
  std::string _e;
  std::vector<Next> _nexts;
  std::vector<Error> _errors;
  std::vector<Finally> _finallies;
  PROMISE_STATUS _status;

public:
  Promise() : _status(PROMISE_STATUS::PENDDING){};
  void resolve() {
    if (_status == PROMISE_STATUS::PENDDING) {
      for (auto &next : _nexts) {
        next();
      }
      for (auto &finally : _finallies) {
        finally();
      }
      _status = PROMISE_STATUS::FULFILLED;
    }
  }
  void resolve(core::AutoPtr<Promise<void>> promise) {
    if (_status == PROMISE_STATUS::PENDDING) {
      promise->next<void>([this]() -> void { resolve(); });
    }
  }
  void reject(const std::string &e) {
    if (_status == PROMISE_STATUS::PENDDING) {
      _e = e;
      for (auto &error : _errors) {
        error(_e);
      }
      for (auto &finally : _finallies) {
        finally();
      }
      _status = PROMISE_STATUS::REJECTED;
      if (_errors.empty()) {
        throw std::runtime_error(e);
      }
    }
  }
  template <class K>
  core::AutoPtr<Promise<K>> next(const std::function<K()> &fn) {
    core::AutoPtr<Promise<K>> pro = new Promise<K>();
    _nexts.push_back([=]() -> void {
      core::AutoPtr<Promise<K>> promise = pro;
      try {
        if constexpr (std::is_void_v<K>) {
          fn();
          promise->resolve();
        } else {
          K res = fn();
          promise->resolve(res);
        }
      } catch (const std::exception &e) {
        promise->reject(e.what());
      }
    });
    return pro;
  }
  template <class K>
  core::AutoPtr<Promise<K>>
  error(const std::function<K(const std::string &e)> &fn) {
    core::AutoPtr<Promise<K>> pro = new Promise<K>();
    auto cb = [=](const std::string &value) -> void {
      core::AutoPtr<Promise<void>> promise = pro;
      if constexpr (std::is_void_v<K>) {
        fn(value);
        promise->resolve();
      } else {
        K res = fn(value);
        promise->resolve(res);
      }
    };
    _errors.push_back(cb);
    if (_status == PROMISE_STATUS::REJECTED) {
      cb(_e);
    }
    return pro;
  }
  void finally(const std::function<void()> &cb) {
    _finallies.push_back(cb);
    if (_status == PROMISE_STATUS::FULFILLED ||
        _status == PROMISE_STATUS::REJECTED) {
      cb();
    }
  }
  void cancel() {
    if (_status == PROMISE_STATUS::PENDDING) {
      _status = PROMISE_STATUS::CANCELED;
    }
  }
};
template <class T> class Promise : public Object {
public:
  using Resolve = std::function<void(const T &value)>;
  using Reject = std::function<void(const std::string &e)>;
  using Next = std::function<void(const T &value)>;
  using Error = std::function<void(const std::string &e)>;
  using Finally = std::function<void()>;

private:
  PROMISE_STATUS _status;
  T _value{};
  std::string _e;
  std::vector<Next> _nexts;
  std::vector<Error> _errors;
  std::vector<Finally> _finallies;

public:
  Promise() : _status(PROMISE_STATUS::PENDDING){};
  void resolve(const T &value) {
    if (_status == PROMISE_STATUS::PENDDING) {
      _value = value;
      for (auto &next : _nexts) {
        next(_value);
      }
      for (auto &finally : _finallies) {
        finally();
      }
      _status = PROMISE_STATUS::FULFILLED;
    }
  }
  void resolve(const Promise<T> &value) {
    if (_status == PROMISE_STATUS::PENDDING) {
      value.next([this](const T &value) -> void { resolve(value); });
    }
  }

  void reject(const std::string &e) {
    if (_status == PROMISE_STATUS::PENDDING) {
      _e = e;
      for (auto &error : _errors) {
        error(_e);
      }
      for (auto &finally : _finallies) {
        finally();
      }
      _status = PROMISE_STATUS::REJECTED;
      if (_errors.empty()) {
        throw std::runtime_error(e);
      }
    }
  }

  template <class K>
  core::AutoPtr<Promise<K>> next(const std::function<K(const T &value)> &fn) {
    core::AutoPtr<Promise<K>> pro = new Promise<K>();
    auto cb = [=](const T &value) -> void {
      core::AutoPtr<Promise<K>> promise = pro;
      try {
        if constexpr (std::is_void_v<K>) {
          fn(value);
          promise->resolve();
        } else {
          K res = fn(value);
          promise->resolve(res);
        }
      } catch (std::exception &e) {
        promise->reject(e.what());
      }
    };
    _nexts.push_back(cb);
    if (_status == PROMISE_STATUS::FULFILLED) {
      cb(_value);
    }
    return pro;
  }
  template <class K>
  core::AutoPtr<Promise<K>>
  error(const std::function<K(const std::string &e)> &fn) {
    core::AutoPtr<Promise<K>> pro = new Promise<K>();
    auto cb = [=](const std::string &value) -> void {
      core::AutoPtr<Promise<K>> promise = pro;
      if constexpr (std::is_void_v<K>) {
        fn(value);
        promise->resolve();
      } else {
        K res = fn(value);
        promise->resolve(res);
      }
    };
    _errors.push_back(cb);
    if (_status == PROMISE_STATUS::REJECTED) {
      cb(_e);
    }
    return pro;
  }

  void finally(const std::function<void()> &cb) {
    _finallies.push_back(cb);
    if (_status == PROMISE_STATUS::FULFILLED ||
        _status == PROMISE_STATUS::REJECTED) {
      cb();
    }
  }

  void cancel() {
    if (_status == PROMISE_STATUS::PENDDING) {
      _status = PROMISE_STATUS::CANCELED;
    }
  }
};
}; // namespace firefly::core