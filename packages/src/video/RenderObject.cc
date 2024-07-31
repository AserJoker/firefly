#include "video/RenderObject.hpp"
#include "core/Singleton.hpp"
#include "video/Renderer.hpp"
using namespace firefly;
using namespace firefly::video;
void RenderObject::enable() {
  core::AutoPtr renderer = core::Singleton<Renderer>::instance();
  renderer->_robjects[getIdentity()] = this;
}
void RenderObject::disable() {
  core::AutoPtr renderer = core::Singleton<Renderer>::instance();
  renderer->_robjects.erase(getIdentity());
}