#include "exception/Exception.hpp"
#include <fmt/core.h>
using namespace firefly;
using namespace firefly::exception;
static std::string format(const std::string &type, const std::string &message,
                          const std::runtime_error caused,
                          const std::source_location &current) {
  return fmt::format("{}:{}\n\tat {}({}:{}:{})\ncaused by:\n{}", type, message,
                     current.function_name(), current.file_name(),
                     current.line(), current.column(), caused.what());
}
static std::string format(const std::string &type, const std::string &message,
                          const std::source_location &current) {
  return fmt::format("{}:{}\n\tat {}({}:{}:{})", type, message,
                     current.function_name(), current.file_name(),
                     current.line(), current.column());
}
Exception::Exception(const std::string &type, const std::string &message,
                     const std::runtime_error caused,
                     const std::source_location &current)
    : std::runtime_error(format(type, message, caused, current)) {}
Exception::Exception(const std::string &type, const std::string &message,
                     const std::source_location &current)
    : std::runtime_error(format(type, message, current)) {}