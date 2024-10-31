#pragma once
#include "core/CompileString.hpp"
#include <exception>
#include <source_location>
#include "Type.hpp"
#include <stdexcept>
namespace firefly::core {
class Exception : public std::runtime_error {
public:
  Exception(
      const core::String_t &type, const core::String_t &message,
      const std::exception &caused,
      const std::source_location &current = std::source_location::current());
  Exception(
      const core::String_t &type, const core::String_t &message,
      const std::source_location &current = std::source_location::current());
};
template <core::CompileString type> class RuntimeException : public Exception {
public:
  RuntimeException(
      const core::String_t &message, const std::exception &caused,
      const std::source_location &current = std::source_location::current())
      : Exception(type.value, message, caused, current) {}
  RuntimeException(
      const core::String_t &message = "",
      const std::source_location &current = std::source_location::current())
      : Exception(type.value, message, current) {}
};
}; // namespace firefly::core