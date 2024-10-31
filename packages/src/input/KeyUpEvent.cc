#include "input/KeyUpEvent.hpp"
using namespace firefly;
using namespace firefly::input;
KeyUpEvent::KeyUpEvent(core::Unsigned_t scancode) : _scancode(scancode) {}
const core::Unsigned_t &KeyUpEvent::getScancode() const { return _scancode; }