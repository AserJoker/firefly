#include "input/KeyDownEvent.hpp"
using namespace firefly::input;
KeyDownEvent::KeyDownEvent(uint32_t scancode) : _scancode(scancode) {}
const uint32_t &KeyDownEvent::getScancode() const { return _scancode; }