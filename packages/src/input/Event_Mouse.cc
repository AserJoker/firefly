#include "input/Event_Mouse.hpp"
using namespace firefly;
using namespace firefly::input;
Event_Mouse::Event_Mouse(const glm::vec2 &position, const glm::vec2 &delta)
    : _position(position), _delta(delta) {}
const glm::vec2 &Event_Mouse::getPosition() const { return _position; }
const glm::vec2 &Event_Mouse::getDelta() const { return _delta; }