#pragma once
#include "core/AutoPtr.hpp"
#include "core/Rect.hpp"
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
  core::Point<> _cameraPosition;
  CameraType _cameraType;

private:
  core::Rect<> getViewport();
  core::AutoPtr<RenderTarget> getRenderTarget();

protected:
  void onAttrChange(const std::string &name) override;

public:
  Scene();
  void onTick() override;
  void setCamera(CameraType type);
  const core::AutoPtr<video::Camera> &getCamera() const;
  CameraType getCameraType() const;
};
}; // namespace firefly::document