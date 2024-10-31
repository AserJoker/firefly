#pragma once
#include "TestException.hpp"
#include "core/CompileString.hpp"
#include "core/Map.hpp"
#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <fmt/format.h>
#include <functional>
#include <regex>

using namespace firefly;
class BaseTestSuit {
private:
  static inline core::Map<core::String_t, BaseTestSuit *> _suits;

public:
  static core::Array<BaseTestSuit *> get(const core::String_t &name) {
    core::Array<BaseTestSuit *> suits;
    if (name.starts_with('/')) {
      std::regex regex(name.substr(1, name.length() - 2));
      for (auto &[sname, suit] : _suits) {
        if (std::regex_match(sname, regex)) {
          suits.pushBack(suit);
        }
      }
    } else {
      if (_suits.contains(name)) {
        suits.pushBack(_suits.at(name));
      }
    }
    return suits;
  }
  static void enableAll() {
    for (auto &[_, suit] : _suits) {
      suit->enable();
    }
  }

protected:
  template <core::CompileString suit> static int before() {
    BaseTestSuit *s = _suits.at(suit.value);
    return s->beforeAll();
  };
  template <core::CompileString suit> static int after() {
    BaseTestSuit *s = _suits.at(suit.value);
    return s->afterAll();
  };
  template <core::CompileString suit, core::CompileString name>
  static void test() {
    BaseTestSuit *s = _suits.at(suit.value);
    Test t = s->_tests.at(name.value);
    int result = 0;
    result = t.before();
    if (result != 0) {
      CU_FAIL_FATAL(fmt::format("Failed to initialize Test '{}' code:'{}'",
                                name.value, result)
                        .c_str());
    }
    t.test();
    result = t.after();
    if (result != 0) {
      CU_FAIL_FATAL(fmt::format("Failed to uninitialize Test '{}' code:'{}'",
                                name.value, result)
                        .c_str());
    }
  };

private:
  struct Test {
    std::function<int()> before;
    std::function<int()> after;
    std::function<void()> test;
  };

private:
  core::String_t _name;
  core::Map<core::String_t, Test> _tests;

public:
  BaseTestSuit(const core::String_t &name) {
    _name = name;
    _suits[_name] = this;
  }
  virtual ~BaseTestSuit() {}

  BaseTestSuit(const BaseTestSuit &another) {
    _name = another._name;
    _tests = another._tests;
    _suits[_name] = this;
  }
  const core::String_t &getSuitName() const { return _name; }

  virtual CU_ErrorCode enable(const core::String_t &name = "") = 0;

protected:
  virtual int beforeAll() { return 0; };
  virtual int afterAll() { return 0; };

  void defineTest(const core::String_t &name, const std::function<void()> &test,
                  const std::function<int()> &before,
                  const std::function<int()> &after) {
    _tests[name] = {before, after, test};
  }
};

template <core::CompileString name, typename T>
class TestSuit : public BaseTestSuit {
private:
  CU_pSuite _suite;
  core::Map<core::String_t, std::function<void()>> _tests;
  core::Array<core::String_t> _enableTests;

private:
  static int defaultBefore() { return 0; }
  static int defaultAfter() { return 0; }

public:
  TestSuit() : BaseTestSuit(name.value) {
    _suite = CU_add_suite(name.value, BaseTestSuit::before<name>,
                          BaseTestSuit::after<name>);
    if (_suite == NULL) {
      throw TestException();
    }
  }

  template <core::CompileString test>
  void defineTest(void (T::*TEST)(), int (T::*before)() = nullptr,
                  int (T::*after)() = nullptr) {
    BaseTestSuit::defineTest(
        test.value,
        std::function<void()>([=, this]() -> void { ((T *)this->*TEST)(); }),
        std::function<int()>([=, this]() -> int {
          if (!before) {
            return 0;
          }
          return ((T *)this->*before)();
        }),
        std::function<int()>([=, this]() -> int {
          if (!after) {
            return 0;
          }
          return ((T *)this->*after)();
        }));
    _tests[test.value] = [=, this]() -> void {
      if (std::find(_enableTests.begin(), _enableTests.end(), test.value) !=
          _enableTests.end()) {
        return;
      }
      _enableTests.pushBack(test.value);
      CU_add_test(_suite, test.value, &BaseTestSuit::test<name, test>);
    };
  }

  CU_ErrorCode enable(const core::String_t &test) override {
    if (test.empty()) {
      for (auto &[_, exec] : _tests) {
        exec();
      }
    } else {
      if (test.starts_with('/')) {
        std::regex regex(test.substr(1, test.length() - 2));
        for (auto &[key, exec] : _tests) {
          if (std::regex_match(key.begin(), key.end(), regex)) {
            exec();
          }
        }
      } else {
        if (!_tests.contains(test)) {
          return CUE_NO_TESTNAME;
        }
        _tests[test]();
      }
    }
    return CUE_SUCCESS;
  }
};