#include "runtime/Event.hpp"

using namespace firefly::runtime;

Event::Event() : _canceled(false) {}

void Event::cancel() { _canceled = true; }

bool Event::isCanceled() const { return _canceled; }