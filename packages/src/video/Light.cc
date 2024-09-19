#include "video/Light.hpp"
#include "video/AmbientLight.hpp"
#include "video/PointLight.hpp"
using namespace firefly;
using namespace firefly::video;
Light::Light() { _ambientLight = new AmbientLight(); }
void Light::active(core::AutoPtr<gl::Constant> &constants) {
  _ambientLight->active(constants);
  uint32_t index = 0;
  for (auto &[_, plight] : _positionLights) {
    plight->active(constants, index++);
  }
  constants->setField("numPointLight", (int32_t)_positionLights.size());
}
core::AutoPtr<AmbientLight> Light::getAmbientLight() { return _ambientLight; }
core::AutoPtr<PointLight> &Light::getPointLight(const std::string &name) {
  if (!_positionLights.contains(name)) {
    _positionLights[name] = new PointLight();
  }
  return _positionLights.at(name);
}