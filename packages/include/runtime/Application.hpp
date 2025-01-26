#pragma once
#include "core/Object.hpp"
#include <SDL2/SDL.h>
#include <vector>

namespace firefly::runtime {
class Application : public core::Object {
private:
  std::vector<std::string> _argv;

public:
  Application();

  int start(int argc, char *argv[]);

  const std::string &argv(uint32_t index) const;
};
} // namespace firefly::runtime