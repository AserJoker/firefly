#pragma once
#include "Constant.hpp"
#include "Geometry.hpp"
#include "Shader.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "video/Image.hpp"
namespace firefly::video {
class Renderer : public core::Object {
private:
  core::AutoPtr<Shader> _shader;
  core::AutoPtr<Constant> _constants;

public:
  Renderer();
  void drawGeomeory(const core::AutoPtr<Geometry> &geometry);
  void activeShader(const core::AutoPtr<Shader> &shader);
  void setConstants(const core::AutoPtr<Constant> &constants);
  void setTexture2D(const core::AutoPtr<Image> image,
                    const uint32_t &index = 0);
  const core::AutoPtr<Constant> &getConstants() const;
  core::AutoPtr<Constant> &getConstants();
};
} // namespace firefly::video