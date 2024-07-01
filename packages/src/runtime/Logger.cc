#include "runtime/Logger.hpp"
#include <chrono>
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <iostream>
using namespace firefly::runtime;
Logger::Logger() : _mask(LEVEL::INFO) {}
void Logger::setMask(LEVEL mask) { _mask = mask; }
Logger::LEVEL Logger::getMask() { return _mask; }
void Logger::write(const std::string &msg) { std::cout << msg << std::endl; }
void Logger::print(LEVEL level, const std::string &msg) {
  if (level < _mask) {
    return;
  }
  std::string slevel;
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
  case LEVEL::ERROR:
    slevel = "ERROR";
    break;
  case LEVEL::PANIC:
    slevel = "PANIC";
    break;
  }
  write(fmt::format("{} [{}]:{}",
                    std::chrono::system_clock::now(), slevel, msg));
}