#pragma once
#include "core/Exception.hpp"
#include <SDL_error.h>
namespace firefly::runtime {
class SDLException : public core::RuntimeException<"SDL"> {
public:
  SDLException(
      const std::string &message = "",
      const std::source_location &current = std::source_location::current())
      : core::RuntimeException<"SDL">(
            message.empty() ? SDL_GetError() : message, current) {}
};
}; // namespace firefly::runtime