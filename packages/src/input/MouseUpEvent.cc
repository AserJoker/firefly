#include "input/MouseUpEvent.hpp"
using namespace firefly;
using namespace firefly::input;
MouseUpEvent::MouseUpEvent(core::Unsigned_t type) : _type(type) {}
const core::Unsigned_t &MouseUpEvent::getType() const { return _type; }