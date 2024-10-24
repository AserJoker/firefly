#include "document/Renderable.hpp"
#include "core/AutoPtr.hpp"
#include "document/Window.hpp"
#include "video/Renderer.hpp"
using namespace firefly;
using namespace firefly::document;
void Renderable::onTick() {
  Node::onTick();
  auto win = findParent<Window>();
  auto renderer = win->getRenderer();
  if (renderer != nullptr) {
    renderer->draw(getMaterial(), getGeometry(), getMatrix());
  }
}