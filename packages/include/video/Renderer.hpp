#pragma once
#include "Shader.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "video/Geometry.hpp"
#include <unordered_map>
namespace firefly::video {
class Object3D;
class Renderer : public core::Object {
private:
  struct RenderBuffer {
    uint32_t buffer;
    uint32_t version;
    uint32_t size;
  };

  struct RenderObject {
    uint32_t version;
    uint32_t vao;
    RenderBuffer ibuffer;
    std::unordered_map<std::string, RenderBuffer> buffers;
  };

private:
  std::unordered_map<std::string, RenderObject> _vaos;
  core::AutoPtr<Shader> _shader;

private:
  void syncToBackend(core::AutoPtr<Geometry> &geometry);

public:
  void setShader(const core::AutoPtr<Shader> &shader);
  void renderGeometry(core::AutoPtr<Geometry> &geometry);
  void render(core::AutoPtr<Object3D> root);
};
} // namespace firefly::video