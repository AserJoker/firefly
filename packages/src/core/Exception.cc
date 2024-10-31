#include "core/Exception.hpp"
#include <fmt/core.h>
using namespace firefly;
using namespace firefly::core;
static core::String_t format(const core::String_t &type,
                             const core::String_t &message,
                             const std::exception &caused,
                             const std::source_location &current) {
  return fmt::format("{}: {}\n\tat {}({}:{}:{})\ncaused by:\n{}", type, message,
                     current.function_name(), current.file_name(),
                     current.line(), current.column(), caused.what());
}
static core::String_t format(const core::String_t &type,
                             const core::String_t &message,
                             const std::source_location &current) {
  return fmt::format("{}: {}\n\tat {}({}:{}:{})", type, message,
                     current.function_name(), current.file_name(),
                     current.line(), current.column());
}
Exception::Exception(const core::String_t &type, const core::String_t &message,
                     const std::exception &caused,
                     const std::source_location &current)
    : std::runtime_error(format(type, message, caused, current)) {}
Exception::Exception(const core::String_t &type, const core::String_t &message,
                     const std::source_location &current)
    : std::runtime_error(format(type, message, current)) {}