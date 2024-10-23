#include "input/ClickEvent.hpp"
using namespace firefly::input;
ClickEvent::ClickEvent(uint32_t type) : _type(type) {}
const uint32_t &ClickEvent::getType() const { return _type; }