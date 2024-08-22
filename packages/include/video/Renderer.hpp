#pragma once
#include "Shader.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "video/Geometry.hpp"
#include <unordered_map>
namespace firefly::video {
class Renderer : public core::Object {
private:
  std::unordered_map<std::string, uint32_t> _vaos;
  std::unordered_map<std::string, uint32_t> _buffers;

  core::AutoPtr<Shader> _shader;

public:
  void setShader(const core::AutoPtr<Shader> &shader);
  void render(core::AutoPtr<Geometry> geometry);
};
} // namespace firefly::video