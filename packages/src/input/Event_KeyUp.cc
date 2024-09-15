#include "input/Event_KeyUp.hpp"
using namespace firefly::input;
Event_KeyUp::Event_KeyUp(uint32_t scancode) : _scancode(scancode) {}
const uint32_t &Event_KeyUp::getScancode() const { return _scancode; }