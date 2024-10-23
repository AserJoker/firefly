#include "input/MouseDownEvent.hpp"
using namespace firefly::input;
MouseDownEvent::MouseDownEvent(uint32_t type) : _type(type) {}
const uint32_t &MouseDownEvent::getType() const { return _type; }