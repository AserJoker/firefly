#pragma once
#include "TestException.hpp"
#include "TestSuit.hpp"
#include <CUnit/Basic.h>
#include <CUnit/TestDB.h>
#include <fmt/format.h>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <vector>
template <typename... SUITS> class Testing {
private:
  std::unordered_map<std::string, std::vector<std::string>> testingTests;

private:
  struct CmdLineOption {
    std::string name;
    std::string alias;
    std::function<void(const std::vector<std::string> &)> exec;
    std::string description;
  };

private:
  std::unordered_map<std::string, CmdLineOption> _options;
  std::unordered_map<std::string, std::string> _alias;

  std::vector<std::string> splitStringWithRegex(const std::string &source,
                                                char spliter) {
    std::vector<std::string> result;
    auto inRegex = false;
    std::string item;
    for (size_t i = 0; i < source.length(); i++) {
      if (source[i] == spliter && !inRegex) {
        if (!item.empty()) {
          result.push_back(item);
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
      result.push_back(item);
    }
    return result;
  }

  void parseArgs(int argc, char *argv[]) {
    for (auto index = 1; index < argc; index++) {
      std::string arg = argv[index];
      if (arg.starts_with("-")) {
        std::string opt;
        std::string value;
        auto pos = arg.find_first_of("=");
        if (pos == std::string::npos) {
          opt = arg;
        } else {
          opt = arg.substr(0, pos);
          value = arg.substr(pos + 1);
        }
        std::vector<std::string> params = splitStringWithRegex(value, ',');
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

  void defineSuits(const std::vector<std::string> &suits) {
    for (auto &suitName : suits) {
      testingTests[suitName] = {};
    }
  }

  void defineTests(const std::vector<std::string> &tests) {
    for (auto &testFullName : tests) {
      std::vector<std::string> params = splitStringWithRegex(testFullName, '.');
      if (params.size() != 2) {
        throw TestException(fmt::format("Invald test name: '{}'", testFullName),
                            CUE_NO_TESTNAME);
      }
      std::string suitName = params[0];
      std::string testName = params[1];
      if (suitName.empty() || testName.empty()) {
        throw TestException(fmt::format("Invald test name: '{}'", testFullName),
                            CUE_NO_TESTNAME);
      }
      auto &testings = testingTests[suitName];
      testings.push_back(testName);
    }
  }

  void help(const std::vector<std::string> &) {}

  template <class Fn>
  void setOption(const std::string &name, const std::string &alias,
                 const std::string &description, Fn exec) {
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
              [this](const std::vector<std::string> &args) -> void {
                defineSuits(args);
              });
    setOption("tests", "t", "set witch tests will be test",
              [this](const std::vector<std::string> &args) -> void {
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