#pragma once
#include "Geometry.hpp"
#include "RenderObject.hpp"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
namespace firefly::video {
class Sprite2D : public RenderObject {
private:
  Geometry::Rect _textureRect;
  Geometry::Rect _rect;
  Geometry::Rotation _rotation;
  std::string _texture;
  glm::mat4 _model;
  glm::mat4 _uv_model;
  float _zIndex;
  std::string _mesh;

  void updateModel();
  void updateUVModel();

public:
  Sprite2D();
  void draw() override;
  const Geometry::Rect &getTextureRect() const;
  const Geometry::Rect &getRect() const;
  const float &getZIndex() const;
  const std::string &getTexture() const;
  const std::string &getMesh() const;
  const Geometry::Rotation &getRotation() const;

  void setTextureRect(const Geometry::Rect &rc);
  void setRect(const Geometry::Rect &rc);
  void setZIndex(const float &z);
  void setTexture(const std::string &texture);
  void setMesh(const std::string &mesh);
  void setRotation(const Geometry::Rotation &rot);
};
}; // namespace firefly::video