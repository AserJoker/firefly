#include "input/Event_MouseMotion.hpp"
using namespace firefly;
using namespace firefly::input;
Event_MouseMotion::Event_MouseMotion(const glm::vec2 &position,
                                     const glm::vec2 &delta)
    : _position(position), _delta(delta) {}
const glm::vec2 &Event_MouseMotion::getPosition() const { return _position; }
const glm::vec2 &Event_MouseMotion::getDelta() const { return _delta; }