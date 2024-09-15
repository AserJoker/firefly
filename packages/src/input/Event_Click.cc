#include "input/Event_Click.hpp"
using namespace firefly::input;
Event_Click::Event_Click(uint32_t type) : _type(type) {}
const uint32_t &Event_Click::getType() const { return _type; }