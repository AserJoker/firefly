#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "runtime/Event_SDL.hpp"
namespace firefly::video {
class Renderer : public core::Object {

public:
private:
  void onEvent(runtime::Event_SDL &event);

public:
  void initialize() override;
  void setClearColor(float r, float g, float b, float a);
  void clear();
  void render();
};
}; // namespace firefly::video