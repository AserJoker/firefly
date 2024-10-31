#include "input/ClickEvent.hpp"
using namespace firefly;
using namespace firefly::input;
ClickEvent::ClickEvent(core::Unsigned_t type) : _type(type) {}
const core::Unsigned_t &ClickEvent::getType() const { return _type; }