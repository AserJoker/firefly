#pragma once
#include "core/AutoPtr.hpp"
#include "core/Point.hpp"
#include "core/Rect.hpp"
#include "core/Size.hpp"
#include "core/Value.hpp"
#include "document/Renderable.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"
#include "video/Texture.hpp"
namespace firefly::document {
class Sprite2D : public Renderable {
private:
  core::AutoPtr<video::Geometry> _geometry;
  core::AutoPtr<video::Material> _material;
  glm::mat4 _matrix;
  core::AutoPtr<video::Texture> _texture;

  core::Rect<> _sourceRect;
  core::Rect<> _destinationRect;
  std::string _texturePath;
  int32_t _zIndex;

private:
  void applyMatrix();
  void applyTexMatrix();

protected:
  const core::AutoPtr<video::Geometry> &getGeometry() const override;
  const core::AutoPtr<video::Material> &getMaterial() const override;
  const glm::mat4 &getMatrix() const override;

  void onAttrChange(const core::String_t &name) override;
  void onLoad() override;

public:
  Sprite2D();

  void setTexture(const core::String_t &path);
  const core::String_t &getTexture() const;

  void setDestinationRect(const core::Rect<> &rect);
  const core::Rect<> &getDestinationRect() const;

  void setDestinationPosition(const core::Point<> &position);
  const core::Point<> &getDestinationPosition() const;

  void setDestinationSize(const core::Size<> &size);
  const core::Size<> &getDestinationSize() const;

  void setSourceRect(const core::Rect<> &rect);
  const core::Rect<> &getSourceRect() const;

  void setSourcePosition(const core::Point<> &position);
  const core::Point<> &getSourcePosition() const;

  void setSourceSize(const core::Size<> &size);
  const core::Size<> &getSourceSize() const;

  void setZIndex(int32_t zIndex);
  int32_t getZIndex() const;

public:
  static inline constexpr auto ATTR_TEXTURE = "texture";
  static inline constexpr auto ATTR_ZINDEX = "zIndex";
  static inline constexpr auto ATTR_DESTINATION = "destination";
  static inline constexpr auto ATTR_DESTINATION_X = "destination.x";
  static inline constexpr auto ATTR_DESTINATION_Y = "destination.y";
  static inline constexpr auto ATTR_DESTINATION_WIDTH = "destination.width";
  static inline constexpr auto ATTR_DESTINATION_HEIGHT = "destination.height";
  static inline constexpr auto ATTR_SOURCE = "source";
  static inline constexpr auto ATTR_SOURCE_X = "source.x";
  static inline constexpr auto ATTR_SOURCE_Y = "source.y";
  static inline constexpr auto ATTR_SOURCE_WIDTH = "source.width";
  static inline constexpr auto ATTR_SOURCE_HEIGHT = "source.height";
};
} // namespace firefly::document