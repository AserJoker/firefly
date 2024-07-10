#include "input/Event_KeyDown.hpp"
using namespace firefly::input;
Event_KeyDown::Event_KeyDown(const uint32_t &scancode) : _scancode(scancode) {}
const uint32_t &Event_KeyDown::getScancode() const { return _scancode; }