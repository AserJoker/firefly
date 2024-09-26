#include "video/Renderable.hpp"
#include "core/Singleton.hpp"
#include "video/Renderer.hpp"
using namespace firefly;
using namespace firefly::video;
void Renderable::onTick() {
  auto renderer = core::Singleton<Renderer>::instance();
  renderer->draw(getMaterial(), getGeometry(), getModelMatrix());
  Node::onTick();
}