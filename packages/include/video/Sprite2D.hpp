#pragma once
#include "Renderable.hpp"
#include "core/AutoPtr.hpp"
#include "gl/AlphaFunc.hpp"
#include "gl/BlendFunc.hpp"
#include "gl/Texture2D.hpp"
#include "video/Material.hpp"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <tuple>
namespace firefly::video {
class Sprite2D : public Renderable {
private:
  glm::ivec4 _dstRect;
  glm::ivec4 _srcRect;
  glm::ivec3 _rotationCenter;
  float _rotationAngle;
  glm::mat4 _matrixModel;
  core::AutoPtr<Material> _material;
  int32_t _zIndex;

  void update();

protected:
  const core::AutoPtr<Geometry> &getGeometry() const override;
  const core::AutoPtr<Material> &getMaterial() const override;
  const glm::mat4 &getMatrixModel() const override;

public:
  Sprite2D(const std::string &path = "");

  void setTexture(const std::string &path);
  void setTexture(const core::AutoPtr<gl::Texture2D> &texture);
  core::AutoPtr<gl::Texture2D> &getTexture();
  const core::AutoPtr<gl::Texture2D> &getTexture() const;

  void setRect(const glm::ivec4 &rect);
  const glm::ivec4 &getRect() const;

  void setPosition(const glm::ivec2 &position);
  const glm::ivec2 getPosition() const;

  void setSize(const glm::ivec2 &size);
  const glm::ivec2 setSize() const;

  void setSourcePosition(const glm::ivec2 &position);
  const glm::ivec2 getSourcePosition() const;

  void setSourceSize(const glm::ivec2 &size);
  const glm::ivec2 getSourceSize() const;

  void setSourceRect(const glm::ivec4 &rect);
  const glm::ivec4 &getSourceRect() const;

  void setRotation(const glm::ivec2 &center, float angle,
                   bool righthandle = false);
  const std::tuple<glm::ivec2, float, bool> getRotation() const;

  void setZIndex(int32_t zIndex);
  int32_t getZIndex() const;

  bool isVisible() const;
  void setVisible(bool visible);

  void setBlend(float blend);
  float getBlend() const;
  const std::pair<gl::BLEND_FUNC, gl::BLEND_FUNC> &getBlendFunc() const;
  void setBlendFunc(gl::BLEND_FUNC dst, gl::BLEND_FUNC src);

  bool isAlphaTest() const;
  void setAlphaTest(bool enable);
  const std::pair<gl::ALPHA_FUNC, float> &getAlphaFunc() const;
  void setAlphaFunc(gl::ALPHA_FUNC func, float data);
};
} // namespace firefly::video