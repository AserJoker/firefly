#pragma once
#include "core/Array.hpp"
#include "core/AutoPtr.hpp"
#include "core/Color.hpp"
#include "core/Point.hpp"
#include "core/Value.hpp"
#include "document/Renderable.hpp"
#include "video/Attribute.hpp"
#include "video/Font.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"
#include "video/Texture.hpp"
#include <string>
namespace firefly::document {
class Font : public Renderable, public Node::Register<"font", Font> {
private:
  struct Text {
    std::wstring text;
    core::Point<> position;
    uint32_t size;
    core::Color<> color;
    bool visible;
  };

private:
  core::AutoPtr<video::Font> _font;
  core::String_t _source;
  core::String_t _shader;
  core::AutoPtr<video::Texture> _texture;
  core::Map<uint32_t, Text> _texts;

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

  void update();

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
  uint32_t createText(const core::String_t &source);
  void setTextSource(uint32_t handle, const core::String_t &source);
  void setTextSize(uint32_t handle, uint32_t size);
  void setTextPosition(uint32_t handle, const core::Point<> &position);
  void setTextColor(uint32_t handle, const core::Color<> &color);
  void setTextVisible(uint32_t handle, bool visible);
  void deleteText(uint32_t handle);

public:
  static constexpr auto PROP_SOURCE = "source";
  static constexpr auto PROP_SHADER = "shader";
};
}; // namespace firefly::document