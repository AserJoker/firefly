#include "video/Renderer.hpp"
#include <glad/glad.h>
using namespace firefly;
using namespace firefly::video;
void Renderer::initialize() {}
void Renderer::clear() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
void Renderer::render() {
  clear();
  for (auto &[_, obj] : _robjects) {
    obj->draw();
  }
}
void Renderer::setClearColor(float r, float g, float b, float a) {
  glClearColor(r, g, b, a);
}
void Renderer::initDefaultResourceSet() {
  
}
void Renderer::bindShader(const std::string &name) {}
void Renderer::bindTexture(const std::string &name, int index) {}
core::AutoPtr<Shader> Renderer::getCurrentShader() { return _currentShader; }
const core::AutoPtr<Mesh> Renderer::getMesh(const std::string &name) const {
  if (_current.meshs.contains(name)) {
    return _current.meshs.at(name);
  }
  if (_default.meshs.contains(name)) {
    return _default.meshs.at(name);
  }
  return nullptr;
}
const core::AutoPtr<Shader> Renderer::getShader(const std::string &name) const {
  if (_current.shaders.contains(name)) {
    return _current.shaders.at(name);
  }
  if (_default.shaders.contains(name)) {
    return _default.shaders.at(name);
  }
  return nullptr;
}
const core::AutoPtr<Texture>
Renderer::getTexture(const std::string &name) const {
  if (_current.textures.contains(name)) {
    return _current.textures.at(name);
  }
  if (_default.textures.contains(name)) {
    return _default.textures.at(name);
  }
  return nullptr;
}
void Renderer::drawMesh(const std::string &name) {
  auto mesh = getMesh(name);
  if (mesh != nullptr) {
    mesh->draw();
  }
}
void Renderer::setResourceSet(const ResourceSet &resources) {
  _current = resources;
}