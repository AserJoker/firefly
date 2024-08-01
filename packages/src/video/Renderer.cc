#include "video/Renderer.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "runtime/Application.hpp"
#include "runtime/EventBus.hpp"
#include "runtime/Event_SDL.hpp"
#include "runtime/Media.hpp"
#include "video/Mesh.hpp"
#include "video/Shader.hpp"
#include "video/Texture.hpp"
#include "video/Texture2D.hpp"
#include "video/VertexFormat.hpp"
#include <SDL2/SDL.h>
#include <SDL_video.h>
#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
using namespace firefly;
using namespace firefly::video;
void Renderer::initialize() {
  auto bus = core::Singleton<runtime::EventBus>::instance();
  bus->on(this, &Renderer::onEvent);
}
void Renderer::clear() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
void Renderer::render() {
  auto app = core::Singleton<runtime::Application>::instance();
  auto win = app->getWindow();
  auto size = win->getSize();
  clear();
  for (auto &[_, obj] : _robjects) {
    auto shader = getShader(obj->getShader());
    if (shader != _currentShader) {
      _currentShader = shader;
      shader->enable();
      _currentShader->setUniform("viewport_size", size);
    }
    obj->draw();
  }
}
void Renderer::setClearColor(float r, float g, float b, float a) {
  glClearColor(r, g, b, a);
}
void Renderer::initDefaultResourceSet() {
  auto media = core::Singleton<runtime::Media>::instance();
  auto vbuffer = media->load("shader::sprite_2d::vertex.glsl")->read();
  std::string vs((const char *)vbuffer->getData(), vbuffer->getSize());
  vbuffer = media->load("shader::sprite_2d::fragment.glsl")->read();
  std::string fs((const char *)vbuffer->getData(), vbuffer->getSize());
  float vertices[] = {1.f, 1.f, 0.0f, 1, 1, 1.f, 0.f, 0.0f, 1, 0,
                      0.f, 0.f, 0.0f, 0, 0, 0.f, 1.f, 0.0f, 0, 1};

  unsigned int indices[] = {0, 1, 3, 1, 2, 3};

  core::AutoPtr<Shader> sprite_2d =
      new Shader({{ShaderType::VERTEX, vs}, {ShaderType::FRAGMENT, fs}});

  core::AutoPtr<Mesh> rect =
      new Mesh({VertexFormat::Position3D, VertexFormat::Texture2D});

  auto no_texture = new Texture2D();
  uint32_t buffer[] = {0xff7f7f7f, 0xffffffff, 0xffffffff, 0xff7f7f7f};
  no_texture->setImage(2, 2, GL_RGBA, buffer);
  rect->setVertexData(vertices, sizeof(vertices));
  rect->setElementData(indices, sizeof(indices));
  _default.shaders["sprite_2d"] = sprite_2d;
  _default.meshs["rect"] = rect;
  _default.textures["no_texture"] = no_texture;
}
void Renderer::bindShader(const std::string &name) {
  auto shader = getShader(name);
  if (_currentShader != shader) {
    _currentShader = shader;
    _currentShader->enable();
  }
}
void Renderer::bindTexture(const std::string &name, int index) {
  auto texture = getTexture(name);
  texture->bind(index);
}
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
void Renderer::onEvent(runtime::Event_SDL &e) {
  auto event = e.getEvent();
  if (event.type == SDL_WINDOWEVENT) {
    if (event.window.event == SDL_WINDOWEVENT_SHOWN ||
        event.window.event == SDL_WINDOWEVENT_RESIZED) {
      SDL_Window *win = SDL_GetWindowFromID(event.window.windowID);
      int w, h;
      SDL_GetWindowSize(win, &w, &h);
      glViewport(0, 0, w, h);
      _currentShader->setUniform("viewport_size", {w, h});
    }
  }
}