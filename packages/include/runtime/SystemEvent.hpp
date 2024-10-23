#pragma once
#include "runtime/Event.hpp"
#include <SDL2/SDL.h>
namespace firefly::runtime {
class SystemEvent : public Event {
private:
  SDL_Event _event;

public:
  SystemEvent(const SDL_Event &e);
  const SDL_Event &getEvent() const;
};
}; // namespace firefly::runtime