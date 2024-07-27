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

void Logger::write(const std::string &msg) { std::cout << msg << std::endl; }

void Logger::print(LEVEL level, const std::string &msg) {
  if (level < _mask) {
    return;
  }
  std::string slevel;
  fmt::text_style style = fmt::fg(fmt::color::white);
  switch (level) {
  case LEVEL::DEBUG:
    slevel = "DEBUG";
    style = fmt::fg(fmt::color::gray);
    break;
  case LEVEL::INFO:
    slevel = "INFO";
    style = fmt::fg(fmt::color::blue);
    break;
  case LEVEL::LOG:
    slevel = "LOG";
    style = fmt::fg(fmt::color::white);
    break;
  case LEVEL::WARN:
    slevel = "WARN";
    style = fmt::fg(fmt::color::yellow);
    break;
  case LEVEL::ERR:
    slevel = "ERROR";
    style = fmt::fg(fmt::color::red);
    break;
  case LEVEL::PANIC:
    slevel = "PANIC";
    style = fmt::fg(fmt::color::white) | fmt::bg(fmt::color::red);
    break;
  }
  write(fmt::format(style, "{} [{}]:{}", std::chrono::system_clock::now(),
                    slevel, msg));
}