#pragma once
#include "core/Array.hpp"
#include "core/AutoPtr.hpp"
#include "core/Color.hpp"
#include "core/Point.hpp"
#include "core/Type.hpp"
#include "document/Renderable.hpp"
#include "video/Attribute.hpp"
#include "video/Font.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"
#include "video/Texture.hpp"

namespace firefly::document {
class Font : public Renderable, public Node::Register<"font", Font> {
private:
  struct Text {
    std::wstring text;
    core::Point<> position;
    core::Unsigned_t size;
    core::Color<> color;
    core::Boolean_t visible;
    size_t index;
  };

private:
  core::AutoPtr<video::Font> _font;
  core::String_t _source;
  core::String_t _shader;
  core::AutoPtr<video::Texture> _texture;
  core::Map<core::Unsigned_t, Text> _texts;
  float _zIndex;

private:
  core::AutoPtr<video::Geometry> _geometry;
  core::AutoPtr<video::Material> _material;
  core::AutoPtr<video::Attribute> _attrInstanceModel;
  core::AutoPtr<video::Attribute> _attrInstanceTexModel;
  core::AutoPtr<video::Attribute> _attrInstanceColor;
  glm::mat4 _matrix;

private:
  void onSourceChange();
  void onShaderChange();
  void onZIndexChange();

  void update();
  void updateText(Text &text);

protected:
  const core::AutoPtr<video::Geometry> &getGeometry() const override;
  const core::AutoPtr<video::Material> &getMaterial() const override;
  const glm::mat4 &getMatrix() const override;

  core::Array<glm::mat4> _coords;
  core::Array<glm::mat4> _matrixs;
  core::Array<glm::vec4> _colors;

protected:
  void onLoad() override;

public:
  Font();
  core::Unsigned_t createText(const core::String_t &source);
  void setTextSource(core::Unsigned_t handle, const core::String_t &source);
  void setTextSize(core::Unsigned_t handle, core::Unsigned_t size);
  void setTextPosition(core::Unsigned_t handle, const core::Point<> &position);
  void setTextColor(core::Unsigned_t handle, const core::Color<> &color);
  void setTextVisible(core::Unsigned_t handle, core::Boolean_t visible);
  void setZIndex(core::Float_t visible);
  void deleteText(core::Unsigned_t handle);

public:
  static constexpr auto PROP_SOURCE = "source";
  static constexpr auto PROP_SHADER = "shader";
  static constexpr auto PROP_ZINDEX = "z-index";
};
}; // namespace firefly::document