#pragma once
#include "core/TemplateCString.hpp"
#include <source_location>
#include <stdexcept>
namespace firefly::exception {
class Exception : public std::runtime_error {
public:
  Exception(
      const std::string &type, const std::string &message,
      const std::runtime_error caused,
      const std::source_location &current = std::source_location::current());
  Exception(
      const std::string &type, const std::string &message,
      const std::source_location &current = std::source_location::current());
};
template <core::template_c_string type>
class RuntimeException : public Exception {
public:
  RuntimeException(
      const std::string &message, const std::runtime_error caused,
      const std::source_location &current = std::source_location::current())
      : Exception(type.value, message, caused, current) {}
  RuntimeException(
      const std::string &message = "",
      const std::source_location &current = std::source_location::current())
      : Exception(type.value, message, current) {}
};
}; // namespace firefly::exception