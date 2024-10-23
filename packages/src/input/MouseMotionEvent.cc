#include "input/MouseMotionEvent.hpp"
using namespace firefly;
using namespace firefly::input;
MouseMotionEvent::MouseMotionEvent(const glm::ivec2 &position,
                                   const glm::ivec2 &delta)
    : _position(position), _delta(delta) {}
const glm::ivec2 &MouseMotionEvent::getPosition() const { return _position; }
const glm::ivec2 &MouseMotionEvent::getDelta() const { return _delta; }