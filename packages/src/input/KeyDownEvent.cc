#include "input/KeyDownEvent.hpp"
using namespace firefly;
using namespace firefly::input;
KeyDownEvent::KeyDownEvent(core::Unsigned_t scancode) : _scancode(scancode) {}
const core::Unsigned_t &KeyDownEvent::getScancode() const { return _scancode; }