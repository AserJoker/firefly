#include "input/Event_MouseUp.hpp"
using namespace firefly::input;
Event_MouseUp::Event_MouseUp(const uint32_t &type) : _type(type) {}
const uint32_t &Event_MouseUp::getType() const { return _type; }