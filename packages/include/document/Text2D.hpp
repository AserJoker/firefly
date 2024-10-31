#pragma once
#include "Node.hpp"
#include "core/Color.hpp"
#include "core/Point.hpp"
namespace firefly::document {
class Text2D : public Node, public Node::Register<"text-2d", Text2D> {
private:
  core::Unsigned_t _handle;
  core::String_t _text;
  core::Color<> _color;
  core::Unsigned_t _size;
  core::Point<> _position;
  core::Boolean_t _visible;

protected:
  void onLoad() override;
  void onUnload() override;

  void onTextChange();
  void onColorChange();
  void onSizeChange();
  void onVisibleChange();
  void onPositionChange();

public:
  Text2D();

public:
  constexpr static auto PROP_TEXT = "text";
  constexpr static auto PROP_FONT_COLOR = "font-color";
  constexpr static auto PROP_FONT_SIZE = "font-size";
  constexpr static auto PROP_POSITION = "position";
  constexpr static auto PROP_VISIBLE = "visible";
};
}; // namespace firefly::document