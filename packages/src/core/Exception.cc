#include "core/Exception.hpp"
#include <fmt/core.h>
using namespace firefly;
using namespace firefly::core;
static std::string format(const std::string &type, const std::string &message,
                          const std::exception caused,
                          const std::source_location &current) {
  return fmt::format("{}: {}\n\tat {}({}:{}:{})\ncaused by:\n{}", type, message,
                     current.function_name(), current.file_name(),
                     current.line(), current.column(), caused.what());
}
static std::string format(const std::string &type, const std::string &message,
                          const std::source_location &current) {
  return fmt::format("{}: {}\n\tat {}({}:{}:{})", type, message,
                     current.function_name(), current.file_name(),
                     current.line(), current.column());
}
Exception::Exception(const std::string &type, const std::string &message,
                     const std::exception caused,
                     const std::source_location &current)
    : std::runtime_error(format(type, message, caused, current)) {}
Exception::Exception(const std::string &type, const std::string &message,
                     const std::source_location &current)
    : std::runtime_error(format(type, message, current)) {}