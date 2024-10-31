#include "runtime/Event.hpp"
using namespace firefly;
using namespace firefly::runtime;

Event::Event() : _canceled(false) {}

void Event::cancel() { _canceled = true; }

core::Boolean_t Event::isCanceled() const { return _canceled; }