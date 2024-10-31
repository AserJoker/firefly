#include "input/MouseDownEvent.hpp"
using namespace firefly;
using namespace firefly::input;
MouseDownEvent::MouseDownEvent(core::Unsigned_t type) : _type(type) {}
const core::Unsigned_t &MouseDownEvent::getType() const { return _type; }