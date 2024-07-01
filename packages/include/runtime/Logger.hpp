#pragma once
#include "core/Object.hpp"
#include <fmt/core.h>
namespace firefly::runtime {
class Logger : public core::Object {
public:
  enum class LEVEL { DEBUG = 0, INFO, LOG, WARN, ERROR, PANIC };

private:
  LEVEL _mask;

private:
  virtual void print(LEVEL level, const std::string &msg);

protected:
  virtual void write(const std::string &msg);

public:
  Logger();

  void setMask(LEVEL level);
  LEVEL getMask();

  template <class... T> void info(fmt::format_string<T...> fmt, T &&...args) {
    print(LEVEL::INFO, fmt::vformat(fmt, fmt::make_format_args(args...)));
  }
  template <class... T> void debug(fmt::format_string<T...> fmt, T &&...args) {
    print(LEVEL::DEBUG, fmt::vformat(fmt, fmt::make_format_args(args...)));
  }
  template <class... T> void log(fmt::format_string<T...> fmt, T &&...args) {
    print(LEVEL::LOG, fmt::vformat(fmt, fmt::make_format_args(args...)));
  }
  template <class... T> void warn(fmt::format_string<T...> fmt, T &&...args) {
    print(LEVEL::WARN, fmt::vformat(fmt, fmt::make_format_args(args...)));
  }
  template <class... T> void error(fmt::format_string<T...> fmt, T &&...args) {
    print(LEVEL::ERROR, fmt::vformat(fmt, fmt::make_format_args(args...)));
  }
  template <class... T> void panic(fmt::format_string<T...> fmt, T &&...args) {
    print(LEVEL::PANIC, fmt::vformat(fmt, fmt::make_format_args(args...)));
  }
};
} // namespace firefly::runtime