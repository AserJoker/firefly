#pragma once
#include "TestException.hpp"
#include "TestSuit.hpp"
#include "core/Array.hpp"
#include "core/Map.hpp"
#include <CUnit/Basic.h>
#include <CUnit/TestDB.h>
#include <fmt/format.h>
#include <stdexcept>
#include <tuple>
template <typename... SUITS> class Testing {
private:
  core::Map<core::String_t, core::Array<core::String_t>> testingTests;

private:
  struct CmdLineOption {
    core::String_t name;
    core::String_t alias;
    std::function<void(const core::Array<core::String_t> &)> exec;
    core::String_t description;
  };

private:
  core::Map<core::String_t, CmdLineOption> _options;
  core::Map<core::String_t, core::String_t> _alias;

  core::Array<core::String_t> splitStringWithRegex(const core::String_t &source,
                                                   char spliter) {
    core::Array<core::String_t> result;
    auto inRegex = false;
    core::String_t item;
    for (size_t i = 0; i < source.length(); i++) {
      if (source[i] == spliter && !inRegex) {
        if (!item.empty()) {
          result.pushBack(item);
        }
        item.clear();
      } else if (source[i] == '/' && (i == 0 || source[i - 1] != '\\')) {
        inRegex = !inRegex;
        item += '/';
      } else {
        item += source[i];
      }
    }
    if (!item.empty()) {
      result.pushBack(item);
    }
    return result;
  }

  void parseArgs(int argc, char *argv[]) {
    for (auto index = 1; index < argc; index++) {
      core::String_t arg = argv[index];
      if (arg.starts_with("-")) {
        core::String_t opt;
        core::String_t value;
        auto pos = arg.find_first_of("=");
        if (pos == core::String_t::npos) {
          opt = arg;
        } else {
          opt = arg.substr(0, pos);
          value = arg.substr(pos + 1);
        }
        core::Array<core::String_t> params = splitStringWithRegex(value, ',');
        if (!opt.starts_with("--")) {
          if (!_alias.contains(opt)) {
            throw std::runtime_error(
                fmt::format("Unknown options '{}',please see help", opt));
          }
          opt = _alias[opt];
        }
        if (!_options.contains(opt)) {
          throw std::runtime_error(
              fmt::format("Unknown options '{}',please see help", opt));
        }
        CmdLineOption &option = _options.at(opt);
        option.exec(params);
      }
    }
  }

  void defineSuits(const core::Array<core::String_t> &suits) {
    for (auto &suitName : suits) {
      testingTests[suitName] = {};
    }
  }

  void defineTests(const core::Array<core::String_t> &tests) {
    for (auto &testFullName : tests) {
      core::Array<core::String_t> params =
          splitStringWithRegex(testFullName, '.');
      if (params.size() != 2) {
        throw TestException(fmt::format("Invald test name: '{}'", testFullName),
                            CUE_NO_TESTNAME);
      }
      core::String_t suitName = params[0];
      core::String_t testName = params[1];
      if (suitName.empty() || testName.empty()) {
        throw TestException(fmt::format("Invald test name: '{}'", testFullName),
                            CUE_NO_TESTNAME);
      }
      auto &testings = testingTests[suitName];
      testings.pushBack(testName);
    }
  }

  void help(const core::Array<core::String_t> &) {}

  template <class Fn>
  void setOption(const core::String_t &name, const core::String_t &alias,
                 const core::String_t &description, Fn exec) {
    _options[fmt::format("--{}", name)] = {
        .name = name, .alias = alias, .exec = exec, .description = description};
    _alias[fmt::format("-{}", alias)] = fmt::format("--{}", name);
  }

public:
  Testing() {
    if (CUE_SUCCESS != CU_initialize_registry()) {
      throw TestException();
    }
    CU_basic_set_mode(CU_BRM_VERBOSE);
    setOption("suits", "s", "set witch suits will be test",
              [this](const core::Array<core::String_t> &args) -> void {
                defineSuits(args);
              });
    setOption("tests", "t", "set witch tests will be test",
              [this](const core::Array<core::String_t> &args) -> void {
                defineTests(args);
              });
  }
  int run(int argc, char *argv[]) {
    std::tuple<SUITS...> suits = std::make_tuple(SUITS{}...);
    parseArgs(argc, argv);
    if (testingTests.empty()) {
      BaseTestSuit::enableAll();
    } else {
      for (auto &[suitName, testings] : testingTests) {
        auto suits = BaseTestSuit::get(suitName);
        if (suits.empty()) {
          fmt::print("Warning: suit name '{}' not match any suits", suitName);
        } else {
          for (auto &suit : suits) {
            if (testings.empty()) {
              suit->enable();
            } else {
              for (auto &testName : testings) {
                suit->enable(testName);
              }
            }
          }
        }
      }
    }
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
  }
};