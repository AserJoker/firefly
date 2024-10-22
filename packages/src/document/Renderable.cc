#include "document/Renderable.hpp"
#include "core/AutoPtr.hpp"
#include "video/Renderer.hpp"
using namespace firefly;
using namespace firefly::document;
void Renderable::onTick() {
  auto renderer = inject<core::AutoPtr<video::Renderer>>();
  if (renderer != nullptr) {
    renderer->draw(getMaterial(), getGeometry(), getMatrix());
  }
  Node::onTick();
}