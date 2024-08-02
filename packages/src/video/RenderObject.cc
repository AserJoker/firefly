#include "video/RenderObject.hpp"
#include "core/Singleton.hpp"
#include "video/Renderer.hpp"
using namespace firefly;
using namespace firefly::video;
RenderObject::RenderObject() { _shader = "sprite_2d"; }
RenderObject::~RenderObject() { disable(); }
void RenderObject::enable() {
  core::AutoPtr renderer = core::Singleton<Renderer>::instance();
  renderer->_robjects[getIdentity()] = this;
}
void RenderObject::disable() {
  core::AutoPtr renderer = core::Singleton<Renderer>::instance();
  renderer->_robjects.erase(getIdentity());
}
const std::string &RenderObject::getShader() const { return _shader; }
void RenderObject::setShader(const std::string &shader) { _shader = shader; }