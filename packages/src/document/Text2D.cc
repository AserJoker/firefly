#include "document/Text2D.hpp"
#include "document/Font.hpp"
using namespace firefly;
using namespace firefly::document;
Text2D::Text2D() : _handle(0), _color(1, 1, 1, 1), _size(24), _visible(true) {
  defineProperty(PROP_TEXT, _text);
  defineProperty(PROP_POSITION, _position);
  defineProperty(PROP_FONT_COLOR, _color);
  defineProperty(PROP_FONT_SIZE, _size);
  defineProperty(PROP_VISIBLE, _visible);

  watchProp(PROP_TEXT, &Text2D::onTextChange);
  watchProp(PROP_POSITION, &Text2D::onPositionChange);
  watchProp(PROP_FONT_COLOR, &Text2D::onColorChange);
  watchProp(PROP_FONT_SIZE, &Text2D::onSizeChange);
  watchProp(PROP_VISIBLE, &Text2D::onVisibleChange);
}
void Text2D::onLoad() {
  auto font = findParent<Font>();
  if (!font) {
    return;
  }
  _handle = font->createText(_text);
  font->setTextColor(_handle, _color);
  font->setTextSize(_handle, _size);
  font->setTextPosition(_handle, _position);
  font->setTextVisible(_handle, _visible);
  Node::onLoad();
}
void Text2D::onUnload() {
  Node::onUnload();
  auto font = findParent<Font>();
  if (!font) {
    return;
  }
  font->deleteText(_handle);
}

void Text2D::onTextChange() {
  auto font = findParent<Font>();
  if (!font) {
    return;
  }
  font->setTextSource(_handle, _text);
}
void Text2D::onColorChange() {
  auto font = findParent<Font>();
  if (!font) {
    return;
  }
  font->setTextColor(_handle, _color);
}
void Text2D::onSizeChange() {
  auto font = findParent<Font>();
  if (!font) {
    return;
  }
  font->setTextSize(_handle, _size);
}
void Text2D::onVisibleChange() {
  auto font = findParent<Font>();
  if (!font) {
    return;
  }
  font->setTextVisible(_handle, _visible);
}
void Text2D::onPositionChange() {
  auto font = findParent<Font>();
  if (!font) {
    return;
  }
  font->setTextPosition(_handle, _position);
}
