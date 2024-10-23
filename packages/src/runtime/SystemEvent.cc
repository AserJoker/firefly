#include "runtime/SystemEvent.hpp"
#include <SDL_events.h>
using namespace firefly::runtime;
SystemEvent::SystemEvent(const SDL_Event &e) : _event(e) {}
const SDL_Event &SystemEvent::getEvent() const { return _event; }