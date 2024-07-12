#include "GameApplication.hpp"
#include "core/AutoPtr.hpp"
#include "input/Event_KeyDown.hpp"
#include "input/Event_Mouse.hpp"
#include "input/Event_MouseButtonDown.hpp"
#include "input/Event_MouseWheel.hpp"
#include "input/Keyboard.hpp"
#include "input/Mouse.hpp"
#include "runtime/Application.hpp"
#include "runtime/EventBus.hpp"
#include "runtime/Resource.hpp"
#include "runtime/Window.hpp"
#include "video/ElementBuffer.hpp"
#include "video/Mesh.hpp"
#include "video/PerspectiveCamera.hpp"
#include "video/Renderer.hpp"
#include "video/Shader.hpp"
#include "video/Texture.hpp"
#include "video/VertexArray.hpp"
#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>

using namespace firefly;
using namespace duskland;
core::AutoPtr<video::Mesh> mesh;
core::AutoPtr<video::Shader> shader;
core::AutoPtr<video::PerspectiveCamera> camera;
core::AutoPtr<input::Keyboard> keyboard;
core::AutoPtr<input::Mouse> mouse;
struct Vertex {
  glm::vec3 position;
  glm::vec2 texcrood;
  glm::vec3 normal;
};
glm::mat4 model(1.0f);
GameApplication::GameApplication(int argc, char *argv[])
    : runtime::Application(argc, argv) {}
core::AutoPtr<video::Mesh>
loadObjMesh(core::AutoPtr<runtime::Resource> resource) {
  auto buffer = resource->read();
  std::string source((char *)buffer->getData(), buffer->getSize());
  std::stringstream ss(source);
  std::vector<glm::vec3> v;
  std::vector<glm::vec2> vt;
  std::vector<glm::vec3> vn;
  std::vector<Vertex> vertices;
  std::vector<video::Face> indices;
  struct VertexMeta {
    int v, vt, vn;
  };
  std::vector<VertexMeta> f;
  while (!ss.eof()) {
    std::string type;
    ss >> type;
    if (type == "v") {
      glm::vec3 position;
      ss >> position.x >> position.y >> position.z;
      v.push_back(position);
    }
    if (type == "vt") {
      glm::vec2 texcrood;
      ss >> texcrood.x >> texcrood.y;
      vt.push_back(texcrood);
    }
    if (type == "vn") {
      glm::vec3 normal;
      ss >> normal.x >> normal.y >> normal.z;
      vn.push_back(normal);
    }
    if (type == "f") {
      VertexMeta meta[3];
      char c;
      ss >> meta[0].v >> c >> meta[0].vn >> c >> meta[0].vt;
      ss >> meta[1].v >> c >> meta[1].vn >> c >> meta[1].vt;
      ss >> meta[2].v >> c >> meta[2].vn >> c >> meta[2].vt;
      f.push_back(meta[0]);
      f.push_back(meta[1]);
      f.push_back(meta[2]);
    }
  }
  for (int i = 0; i < f.size(); i += 3) {
    Vertex vertex;
    video::Face face;
    vertex.position = v[f[i].v - 1];
    vertex.texcrood = vt[f[i].vt - 1];
    vertex.normal = vn[f[i].vn - 1];
    vertices.push_back(vertex);
    face.indices[0] = vertices.size() - 1;
    vertex.position = v[f[i + 1].v - 1];
    vertex.texcrood = vt[f[i + 1].vt - 1];
    vertex.normal = vn[f[i + 1].vn - 1];
    vertices.push_back(vertex);
    face.indices[1] = vertices.size() - 1;
    vertex.position = v[f[i + 2].v - 1];
    vertex.texcrood = vt[f[i + 2].vt - 1];
    vertex.normal = vn[f[i + 2].vn - 1];
    vertices.push_back(vertex);
    face.indices[2] = vertices.size() - 1;
    indices.push_back(face);
  }
  return new video::Mesh(
      {video::POSITION_XYZ, video::TEXCROOD_UV, video::NORMAL_XYZ}, vertices);
}
void GameApplication::onInitialize() {
  runtime::Application::onInitialize();
  _resources->setCurrentWorkspaceDirectory(cwd().append("media").string());
  _eventbus->on(this, &GameApplication::onMouse);
  _eventbus->on(this, &GameApplication::onKeydown);
  _eventbus->on(this, &GameApplication::onMouseButtonDown);
  _eventbus->on(this, &GameApplication::onMouseWheel);

  _window = new runtime::Window("duskland", 1024, 768);
  _renderer = new video::Renderer();
  _renderer->enableDepthTest();
  shader = new video::Shader(
      {{video::ShaderType::VERTEX, _resources->load("shader::vertex.glsl")},
       {video::ShaderType::FRAGMENT,
        _resources->load("shader::fragment.glsl")}});

  mesh = loadObjMesh(_resources->load("model::model.obj"));
  mesh->getMaterial().diffuse =
      new video::Texture(_resources->load("texture::container2.png"));
  mesh->getMaterial().shininess = 64.0f;
  mesh->getMaterial().specular =
      new video::Texture(_resources->load("texture::container2_specular.png"));
  _renderer->useShader(shader);
  camera = new video::PerspectiveCamera(_window);
  camera->setPosition({0, 0, 0});
  camera->setFront({0, 0, 1});
  _renderer->setCamera(camera);
  keyboard = new input::Keyboard();
  mouse = new input::Mouse();
  model = glm::translate(model, glm::vec3(-0.5, -0.5, 3));
}

void GameApplication::onMainLoop() {
  runtime::Application::onMainLoop();
  if (keyboard->getKeyState(SDL_SCANCODE_A)) {
    camera->move(-0.001, 0, 0);
  }
  if (keyboard->getKeyState(SDL_SCANCODE_D)) {
    camera->move(0.001, 0, 0);
  }
  if (keyboard->getKeyState(SDL_SCANCODE_W)) {
    camera->move(0, 0, 0.001);
  }
  if (keyboard->getKeyState(SDL_SCANCODE_S)) {
    camera->move(0, 0, -0.001);
  }
  if (keyboard->getKeyState(SDL_SCANCODE_LSHIFT)) {
    camera->move(0, -0.001, 0);
  }
  if (keyboard->getKeyState(SDL_SCANCODE_SPACE)) {
    camera->move(0, 0.001, 0);
  }
  _renderer->clear({0.2f, 0.3f, 0.3f, 1.0f});
  shader->setValue("projection", camera->getProjectionMatrix());
  shader->setValue("view", camera->getViewMatrix());
  model = glm::rotate(model, glm::radians(0.01f), glm::vec3(1, 1, 1));
  shader->setValue("model", model);
  shader->setValue("viewPos", camera->getPosition());
  auto &material = mesh->getMaterial();

  _renderer->setTextureUnit("material.diffuse", 0, material.diffuse);
  _renderer->setTextureUnit("material.specular", 1, material.specular);
  shader->setValue("material.shininess", material.shininess);

  shader->setValue("light.position", 0, 0, 0);
  shader->setValue("light.ambient", 0.2f, 0.2f, 0.2f);
  shader->setValue("light.diffuse", 0.8f, 0.8f, 0.8f);
  shader->setValue("light.specular", 1.0f, 1.0f, 1.0f);

  _renderer->draw(mesh);
  _window->present();
}

void GameApplication::onUnInitialize() { runtime::Application::onInitialize(); }

void GameApplication::onMouse(input::Event_Mouse &e) {
  if (mouse->isCaptured()) {
    static float yaw = 90;
    static float pitch = 0;
    float xoffset = e.getDelta().x;
    float yoffset = -e.getDelta().y;
    float sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    yaw += xoffset;
    pitch += yoffset;
    if (pitch > 89.0f) {
      pitch = 89.0f;
    }
    if (pitch < -89.0f) {
      pitch = -89.0f;
    }
    camera->rotate(pitch, yaw);
  }
}

void GameApplication::onMouseButtonDown(input::Event_MouseButtonDown &e) {}

void GameApplication::onKeydown(input::Event_KeyDown &e) {
  if (e.getScancode() == SDL_SCANCODE_ESCAPE) {
    mouse->releaseMouse();
  }
  if (e.getScancode() == SDL_SCANCODE_E) {
    if (mouse->isCaptured()) {
      mouse->releaseMouse();
    } else {
      mouse->captureMouse();
    }
  }
}

void GameApplication::onMouseWheel(input::Event_MouseWheel &e) {
  camera->zoom(camera->getFov() - e.getDelta().y);
}