#pragma once
#include "RenderObject.hpp"
#include "core/Object.hpp"
#include <string>
#include <unordered_map>
namespace firefly::video {
class Renderer : public core::Object {
  friend class RenderObject;

private:
  std::unordered_map<std::string, RenderObject *> _robjects;

public:
  void initialize() override;
  void clear();
  void setClearColor(float r, float g, float b, float a);
  void render();
  void useShader(const std::string &shader);
};
}; // namespace firefly::video