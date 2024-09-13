#pragma once
#include "AmbientLight.hpp"
#include "PointLight.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "gl/Constant.hpp"
#include <map>
namespace firefly::video {
class Light : public core::Object {
private:
  core::AutoPtr<AmbientLight> _ambientLight;
  std::map<std::string, core::AutoPtr<PointLight>> _positionLights;

public:
  Light();
  void active(core::AutoPtr<gl::Constant> &constants);
  core::AutoPtr<AmbientLight> getAmbientLight();
  core::AutoPtr<PointLight> &getPointLight(const std::string &name);
};
} // namespace firefly::video