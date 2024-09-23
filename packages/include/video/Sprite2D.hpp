#pragma once
#include "Renderable.hpp"
#include "core/AutoPtr.hpp"
#include "gl/Texture2D.hpp"
#include "video/Material.hpp"
#include <glm/glm.hpp>
namespace firefly::video {
class Sprite2D : public Renderable {
private:
  glm::ivec4 _dstRect;
  glm::ivec4 _srcRect;
  glm::ivec3 _rotationCenter;
  float _rotationAngle;
  glm::mat4 _matrixModel;
  core::AutoPtr<Material> _material;

  void update();

public:
  Sprite2D(const std::string &path);
  void setTexture(const std::string &path);
  core::AutoPtr<gl::Texture2D> &getTexture();
  const core::AutoPtr<gl::Texture2D> &getTexture() const;
  void setRect(const glm::ivec4 &rect);
  void setSourceRect(const glm::ivec4 &rect);
  void setRotation(const glm::ivec2 &center, float angle,
                   bool righthandle = false);

protected:
  const core::AutoPtr<Geometry> &getGeometry() const override;
  const core::AutoPtr<Material> &getMaterial() const override;
  const glm::mat4 &getModelMatrix() const override;
};
} // namespace firefly::video