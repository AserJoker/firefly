#include "document/Scene.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "runtime/Application.hpp"
#include "video/OrthoCamera.hpp"
#include "video/PerspectiveCamera.hpp"
#include "video/Renderer.hpp"
#include <SDL_video.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
using namespace firefly;
using namespace firefly::document;

core::AutoPtr<Scene> Scene::scene = nullptr;

core::AutoPtr<RenderTarget> Scene::getRenderTarget() {
  auto parent = getParent();
  while (parent != nullptr) {
    auto rt = parent.cast<RenderTarget>();
    if (rt != nullptr) {
      return rt;
    }
    parent = parent->getParent();
  }
  return nullptr;
}
glm::ivec4 Scene::getViewport() {
  auto parent = getParent();
  while (parent != nullptr) {
    auto rt = parent.cast<RenderTarget>();
    if (rt != nullptr) {
      return {0, 0, rt->getRenderTarget()->getSize()};
    }
    parent = parent->getParent();
  }
  auto app = core::Singleton<runtime::Application>::instance();
  auto win = app->getWindow();
  auto size = win->getSize();
  return {0, 0, size};
}
void Scene::onTick() {
  auto renderer = core::Singleton<video::Renderer>::instance();
  auto renderTarget = getRenderTarget();
  Node::onTick();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glm::vec4 viewport = getViewport();
  auto target = getRenderTarget();
  if (_camera != nullptr) {
    _camera->setViewport(viewport);
    auto constants = renderer->getConstants();
    if (target != nullptr) {
      constants->setField("flip", 1);
    } else {
      constants->setField("flip", 0);
    }
    constants->setField("projection", _camera->getProjectionMatrix());
    auto position = _camera->getPosition();
    constants->setField("cameraPosition", position);
  }
  renderer->setViewport(viewport);
  renderer->present();
}
void Scene::onAttrChange(const std::string &name) {
  if (name == "camera" || name.starts_with("camera.")) {
    _camera->setPosition(_cameraPosition);
  }
}
Scene::Scene() : _cameraType(CameraType::NIL) {
  setCamera(CameraType::ORTHO);
  defineAttribute("camera.x", &_cameraPosition.x);
  defineAttribute("camera.y", &_cameraPosition.y);
  defineAttribute("camera.z", &_cameraPosition.z);
}
void Scene::setCamera(CameraType type) {
  _cameraType = type;
  switch (type) {
  case CameraType::ORTHO:
    _camera = core::Singleton<video::OrthoCamera>::instance();
    break;
  case CameraType::PERSPECTIVE:
    _camera = core::Singleton<video::PerspectiveCamera>::instance();
    break;
  case CameraType::NIL:
    _camera = nullptr;
    break;
  }
}
const core::AutoPtr<video::Camera> &Scene::getCamera() const { return _camera; }
