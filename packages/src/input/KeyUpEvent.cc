#include "input/KeyUpEvent.hpp"
using namespace firefly::input;
KeyUpEvent::KeyUpEvent(uint32_t scancode) : _scancode(scancode) {}
const uint32_t &KeyUpEvent::getScancode() const { return _scancode; }