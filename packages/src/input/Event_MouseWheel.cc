#include "input/Event_MouseWheel.hpp"
using namespace firefly::input;
Event_MouseWheel::Event_MouseWheel(const glm::vec2 &position,
                                   const glm::vec2 &delta)
    : _position(position), _delta(delta) {}
const glm::vec2 &Event_MouseWheel::getOffset() const { return _position; }
const glm::vec2 &Event_MouseWheel::getDelta() const { return _delta; }