#include "document/Renderable.hpp"
#include "core/Singleton.hpp"
#include "video/Renderer.hpp"
using namespace firefly;
using namespace firefly::document;
void Renderable::onTick() {
  auto renderer = core::Singleton<video::Renderer>::instance();
  renderer->draw(getMaterial(), getGeometry(), getMatrixModel());
  Node::onTick();
}