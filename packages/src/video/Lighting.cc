#include "video/Lighting.hpp"
using namespace firefly::video;
Lighting::Lighting() : _ambient({0, 0, 0}, 0.2f) {}

Ambient &Lighting::getAmbient() { return _ambient; }
const Ambient &Lighting::getAmbient() const { return _ambient; }