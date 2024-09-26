#pragma once
#include "core/AutoPtr.hpp"
#include "video/Camera.hpp"
#include "video/Node.hpp"
#include "video/RenderTarget.hpp"
namespace firefly::video {
class Scene : public Node {
public:
  enum class CameraType { NIL, ORTHO, PERSPECTIVE };

private:
  core::AutoPtr<Camera> _camera;
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
}; // namespace firefly::video