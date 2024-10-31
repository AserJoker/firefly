#include "runtime/Logger.hpp"
#include <chrono>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <iostream>

using namespace firefly::runtime;

Logger::Logger() : _mask(LEVEL::INFO) {}

void Logger::setMask(LEVEL mask) { _mask = mask; }

Logger::LEVEL Logger::getMask() { return _mask; }

void Logger::write(const core::String_t &msg) { std::cout << msg << std::endl; }

void Logger::print(LEVEL level, const core::String_t &msg) {
  if (level < _mask) {
    return;
  }
  core::String_t slevel;
  switch (level) {
  case LEVEL::DEBUG:
    slevel = "DEBUG";
    break;
  case LEVEL::INFO:
    slevel = "INFO";
    break;
  case LEVEL::LOG:
    slevel = "LOG";
    break;
  case LEVEL::WARN:
    slevel = "WARN";
    break;
  case LEVEL::ERR:
    slevel = "ERROR";
    break;
  case LEVEL::PANIC:
    slevel = "PANIC";
    break;
  }
  write(
      fmt::format("{} [{}]:{}", std::chrono::system_clock::now(), slevel, msg));
}