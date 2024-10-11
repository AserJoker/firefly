#pragma once
#include "core/AutoPtr.hpp"
#include "document/Node.hpp"
#include "document/RenderTarget.hpp"
#include "video/Camera.hpp"

namespace firefly::document {
class Scene : public Node {
public:
  static core::AutoPtr<Scene> scene;

public:
  enum class CameraType { NIL, ORTHO, PERSPECTIVE };

private:
  core::AutoPtr<video::Camera> _camera;
  CameraType _cameraType;

private:
  glm::ivec4 getViewport();
  core::AutoPtr<RenderTarget> getRenderTarget();

public:
  Scene();
  void onTick() override;
  void setCamera(CameraType type);
  CameraType getCameraType() const;
};
}; // namespace firefly::document