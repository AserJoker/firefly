#pragma once
#include "Renderable.hpp"
#include "core/AutoPtr.hpp"
#include "core/Rect.hpp"
#include "gl/AlphaFunc.hpp"
#include "gl/BlendFunc.hpp"
#include "gl/Texture2D.hpp"
#include "video/Material.hpp"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <tuple>
namespace firefly::document {
class Sprite2D : public Renderable {
private:
  core::Rect<> _dstRect;
  core::Rect<> _srcRect;
  core::Point<> _rotationCenter;
  float _rotationAngle;
  int32_t _zIndex;
  std::string _texture;
  bool _visible;

  bool _blendEnable;
  float _blendValue;
  uint32_t _blendSourceFunc;
  uint32_t _blendDestinationFunc;

  bool _alphaTestEnable;
  uint32_t _alphaTestFunc;
  float _alphaTestValue;

  glm::mat4 _matrixModel;
  core::AutoPtr<video::Material> _material;

  void updateModel();
  void updateTexModel();

protected:
  const core::AutoPtr<video::Geometry> &getGeometry() const override;
  const core::AutoPtr<video::Material> &getMaterial() const override;
  const glm::mat4 &getMatrixModel() const override;

  void onAttrChange(const std::string &name) override;

public:
  Sprite2D(const std::string &path = "");

  void setTexture(const std::string &path);
  void setTexture(const core::AutoPtr<gl::Texture2D> &texture);
  const std::string &getTexture() const;
  const core::AutoPtr<gl::Texture2D> &getTextureObject() const;

  void setRect(const core::Rect<> &rect);
  const core::Rect<> &getRect() const;

  void setSourceRect(const core::Rect<> &rect);
  const core::Rect<> &getSourceRect() const;

  void setPosition(const core::Point<> &position);
  const core::Point<> getPosition() const;

  void setSize(const core::Size<> &size);
  const core::Size<> setSize() const;

  void setSourcePosition(const core::Point<> &position);
  const core::Point<> getSourcePosition() const;

  void setSourceSize(const core::Size<> &size);
  const core::Size<> getSourceSize() const;

  void setRotation(const core::Point<> &center, float angle);
  const std::tuple<core::Point<>, float> getRotation() const;

  void setZIndex(int32_t zIndex);
  int32_t getZIndex() const;

  bool isVisible() const;
  void setVisible(bool visible);

  void setBlend(float blend);
  float getBlend() const;
  const std::pair<gl::BLEND_FUNC, gl::BLEND_FUNC> &getBlendFunc() const;
  void setBlendFunc(gl::BLEND_FUNC src, gl::BLEND_FUNC dst);

  bool isAlphaTest() const;
  void setAlphaTest(bool enable);
  const std::pair<gl::ALPHA_FUNC, float> &getAlphaFunc() const;
  void setAlphaFunc(gl::ALPHA_FUNC func, float data);

public:
  static constexpr inline const char *ATTR_RECT = "rect";
  static constexpr inline const char *ATTR_RECT_X = "rect.x";
  static constexpr inline const char *ATTR_RECT_Y = "rect.y";
  static constexpr inline const char *ATTR_RECT_WIDTH = "rect.width";
  static constexpr inline const char *ATTR_RECT_HEIGHT = "rect.height";
  static constexpr inline const char *ATTR_SOURCE = "source";
  static constexpr inline const char *ATTR_SOURCE_X = "source.x";
  static constexpr inline const char *ATTR_SOURCE_Y = "source.y";
  static constexpr inline const char *ATTR_SOURCE_WIDTH = "source.width";
  static constexpr inline const char *ATTR_SOURCE_HEIGHT = "source.height";
  static constexpr inline const char *ATTR_ROTATION = "rotation";
  static constexpr inline const char *ATTR_ROTATION_X = "rotation.x";
  static constexpr inline const char *ATTR_ROTATION_Y = "rotation.y";
  static constexpr inline const char *ATTR_ROTATION_ANGLE = "rotation.angle";
  static constexpr inline const char *ATTR_Z_INDEX = "zIndex";
  static constexpr inline const char *ATTR_VISIBLE = "visible";
  static constexpr inline const char *ATTR_TEXTURE = "texture";

  static constexpr inline const char *ATTR_BLEND = "blend";
  static constexpr inline const char *ATTR_BLEND_ENABLE = "blend.enable";
  static constexpr inline const char *ATTR_BLEND_VALUE = "blend.value";
  static constexpr inline const char *ATTR_BLEND_SOURCE_FUNC =
      "blend.sourceFunc";
  static constexpr inline const char *ATTR_BLEND_DESTINATION_FUNC =
      "blend.destinationFunc";
  static constexpr inline const char *ATTR_ALPHA_TEST = "alphaTest";
  static constexpr inline const char *ATTR_ALPHA_TEST_ENABLE =
      "alphaTest.enable";
  static constexpr inline const char *ATTR_ALPHA_TEST_FUNC = "alphaTest.func";
  static constexpr inline const char *ATTR_ALPHA_TEST_VALUE = "alphaTest.value";
};
} // namespace firefly::document