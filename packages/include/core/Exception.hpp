#pragma once
#include <fmt/format.h>
#include <source_location>
#include <stdexcept>
namespace firefly::core {
class Exception : public std::runtime_error {
private:
  static std::string format(const std::string &message,
                            const std::source_location &loc) {
    return fmt::format("{} \n  at {}({}:{}:{})", message, loc.function_name(),
                       loc.file_name(), loc.line(), loc.column());
  }

public:
  Exception(const std::string &message,
            const std::source_location &loc = std::source_location::current())
      : std::runtime_error(format(message, loc)) {}
};
}; // namespace firefly::core