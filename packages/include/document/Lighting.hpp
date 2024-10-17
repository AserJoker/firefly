#pragma once
#include "Component.hpp"
#include "core/AutoPtr.hpp"
#include "core/Injector.hpp"
#include "core/Map.hpp"
#include "core/Object.hpp"
#include "gl/Texture2D.hpp"
#include "runtime/EventBus.hpp"
#include "runtime/Event_Resize.hpp"
#include <glm/fwd.hpp>
namespace firefly::document {
class Lighting : public core::Object {
public:
  struct Light {
    glm::vec2 _position;
    glm::vec3 _color;
  };

private:
  glm::vec3 _ambientColor;
  float _ambientStrength;
  core::Map<std::string, Light> _lights;
  core::AutoPtr<gl::Texture2D> _lightTexture;
  glm::vec4 _viewport;

private:
  core::Injector<runtime::EventBus, INJECTOR_EVENTBUS> _bus;

private:
  void onResize(runtime::Event_Resize &e);

public:
  Lighting();
  void setAmbient(const glm::vec3 &color, float strength);
  void setLight(const std::string &light, const glm::vec2 &position,
                const glm::vec3 &color);
  void removeLight(const std::string &light);
  void updateTexture();
};
} // namespace firefly::document