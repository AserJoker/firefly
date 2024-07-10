#pragma once
#include "runtime/Event.hpp"
#include <SDL2/SDL.h>
namespace firefly::runtime {
class Event_SDL : public Event {
private:
  SDL_Event _event;

public:
  Event_SDL(const SDL_Event &e);
  const SDL_Event &getEvent() const;
};
}; // namespace firefly::runtime