#include "core/Event.hpp"
using namespace firefly;
using namespace firefly::core;
Event::Event(const std::string &type) : _type(type) {}
void Event::setType(const std::string &type) { _type = type; }
const std::string &Event::getType() const { return _type; }