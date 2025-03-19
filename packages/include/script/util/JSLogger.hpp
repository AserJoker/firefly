#pragma once
#include "JSAllocator.hpp"
#include <chrono>
#include <iostream>
#include <string>

class JSLogger {
public:
  enum class LEVEL { DEBUG = 0, INFO, LOG, WARN, ERROR, PANIC };

private:
  LEVEL _mask{LEVEL::LOG};
  JSAllocator *_allocator;

protected:
  virtual void write(const std::wstring &level, const std::wstring &msg) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto puttime = std::put_time(std::localtime(&time), L"%Y-%M-%D %H:%M:%S");
    std::wstringstream ss;
    ss << puttime;
    std::wcout << std::format(L"{} [{}]: {}", ss.str(), level, msg)
               << std::endl;
  };

public:
  JSLogger(JSAllocator *allocator) : _allocator(allocator) {}

  virtual ~JSLogger() {}

  JSAllocator *getAllocator() { return _allocator; }

  void setMask(const LEVEL &level) { _mask = level; }

  const LEVEL &getMask() const { return _mask; }

  template <class... ARGS>
  void debug(std::wformat_string<ARGS...> fmt, ARGS &&...args) {
    if (getMask() >= LEVEL::DEBUG) {
      return;
    }
    write(L"DEBUG", std::format(fmt, std::forward<ARGS>(args)...));
  }
  template <class... ARGS> void info(const std::wstring &fmt, ARGS... args) {
    if (getMask() >= LEVEL::INFO) {
      return;
    }
    write(L"INFO", std::format(fmt, std::forward<ARGS>(args)...));
  }
  template <class... ARGS> void log(const std::wstring &fmt, ARGS... args) {
    if (getMask() >= LEVEL::LOG) {
      return;
    }
    write(L"LOG", std::format(fmt, std::forward<ARGS>(args)...));
  }
  template <class... ARGS> void warn(const std::wstring &fmt, ARGS... args) {
    if (getMask() >= LEVEL::WARN) {
      return;
    }
    write(L"WARN", std::format(fmt, std::forward<ARGS>(args)...));
  }
  template <class... ARGS> void error(const std::wstring &fmt, ARGS... args) {
    if (getMask() >= LEVEL::ERROR) {
      return;
    }
    write(L"ERROR", std::format(fmt, std::forward<ARGS>(args)...));
  }
  template <class... ARGS> void panic(const std::wstring &fmt, ARGS... args) {
    if (getMask() >= LEVEL::PANIC) {
      return;
    }
    write(L"PAINC", std::format(fmt, std::forward<ARGS>(args)...));
  }
};