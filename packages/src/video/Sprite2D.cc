#include "video/Sprite2D.hpp"
using namespace firefly;
using namespace firefly::video;
void Sprite2D::draw() {}
const Geometry::Rect &Sprite2D::source() const { return _source; }
const Geometry::Rect &Sprite2D::target() const { return _target; }
const float &Sprite2D::zIndex() const { return _zIndex; }
const core::AutoPtr<Texture2D> &Sprite2D::texture() const { return _texture; }