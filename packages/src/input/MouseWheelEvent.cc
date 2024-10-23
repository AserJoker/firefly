#include "input/MouseWheelEvent.hpp"
using namespace firefly::input;
MouseWheelEvent::MouseWheelEvent(const glm::vec2 &position,
                                 const glm::vec2 &delta)
    : _position(position), _delta(delta) {}
const glm::vec2 &MouseWheelEvent::getOffset() const { return _position; }
const glm::vec2 &MouseWheelEvent::getDelta() const { return _delta; }