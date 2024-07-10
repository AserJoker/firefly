#include "runtime/Event_SDL.hpp"
#include <SDL_events.h>
using namespace firefly::runtime;
Event_SDL::Event_SDL(const SDL_Event &e) : _event(e) {}
const SDL_Event &Event_SDL::getEvent() const { return _event; }