#include "input/Event_MouseButtonDown.hpp"
using namespace firefly::input;
Event_MouseButtonDown::Event_MouseButtonDown(const uint32_t &type)
    : _type(type) {}
const uint32_t &Event_MouseButtonDown::getType() const { return _type; }