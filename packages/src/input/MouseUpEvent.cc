#include "input/MouseUpEvent.hpp"
using namespace firefly::input;
MouseUpEvent::MouseUpEvent(uint32_t type) : _type(type) {}
const uint32_t &MouseUpEvent::getType() const { return _type; }