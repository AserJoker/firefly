#include "input/Event_MouseDown.hpp"
using namespace firefly::input;
Event_MouseDown::Event_MouseDown(const uint32_t &type)
    : _type(type) {}
const uint32_t &Event_MouseDown::getType() const { return _type; }