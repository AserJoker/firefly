#pragma once
#include "Constant.hpp"
#include "Geometry.hpp"
#include "Material.hpp"
#include "RenderObject.hpp"
#include "Scene.hpp"
#include "Shader.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "video/Image.hpp"
#include <vector>
namespace firefly::video {
class Renderer : public core::Object {
private:
  core::AutoPtr<Constant> _constants;
  std::vector<core::AutoPtr<RenderObject>> _normalRenderList;
  std::vector<core::AutoPtr<RenderObject>> _blendRenderList;

public:
  Renderer();
  void drawGeomeory(const core::AutoPtr<Geometry> &geometry);
  void setShader(const core::AutoPtr<Shader> &shader);
  void setTexture2D(const core::AutoPtr<Image> &image,
                    const uint32_t &index = 0);
  void setMaterial(const core::AutoPtr<Material> &material);
  const core::AutoPtr<Constant> &getConstants() const;
  core::AutoPtr<Constant> &getConstants();
  void draw(const core::AutoPtr<Material> &material,
            const core::AutoPtr<Geometry> &geometry);
  void render(core::AutoPtr<Scene> &scene);
};
} // namespace firefly::video