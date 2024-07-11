#include "GameApplication.hpp"
#include "core/AutoPtr.hpp"
#include "input/Event_KeyDown.hpp"
#include "input/Event_Mouse.hpp"
#include "input/Event_MouseButtonDown.hpp"
#include "input/Keyboard.hpp"
#include "input/Mouse.hpp"
#include "runtime/Application.hpp"
#include "runtime/Resource_Buffer.hpp"
#include "runtime/Window.hpp"
#include "video/Mesh.hpp"
#include "video/PerspectiveCamera.hpp"
#include "video/Renderer.hpp"
#include "video/Shader.hpp"
#include "video/Texture.hpp"
#include "video/VertexArray.hpp"
#include <SDL_keycode.h>
#include <SDL_scancode.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <initializer_list>

using namespace firefly;
using namespace duskland;
core::AutoPtr<video::VertexArray> vertexarray;
core::AutoPtr<video::Mesh> mesh;
core::AutoPtr<video::Shader> shader;
core::AutoPtr<video::Texture> texture;
core::AutoPtr<video::Texture> texture2;
core::AutoPtr<video::PerspectiveCamera> camera;
core::AutoPtr<input::Keyboard> keyboard;
core::AutoPtr<input::Mouse> mouse;
struct Vertex {
  float x, y, z, u, v;
};
glm::mat4 projection(1.0f), view(1.0f), model(1.0f);
std::initializer_list<Vertex> vertices = {
    {-0.5f, -0.5f, -0.5f, 0.0f, 0.0f}, {0.5f, -0.5f, -0.5f, 1.0f, 0.0f},
    {0.5f, 0.5f, -0.5f, 1.0f, 1.0f},   {0.5f, 0.5f, -0.5f, 1.0f, 1.0f},
    {-0.5f, 0.5f, -0.5f, 0.0f, 1.0f},  {-0.5f, -0.5f, -0.5f, 0.0f, 0.0f},

    {-0.5f, -0.5f, 0.5f, 0.0f, 0.0f},  {0.5f, -0.5f, 0.5f, 1.0f, 0.0f},
    {0.5f, 0.5f, 0.5f, 1.0f, 1.0f},    {0.5f, 0.5f, 0.5f, 1.0f, 1.0f},
    {-0.5f, 0.5f, 0.5f, 0.0f, 1.0f},   {-0.5f, -0.5f, 0.5f, 0.0f, 0.0f},

    {-0.5f, 0.5f, 0.5f, 1.0f, 0.0f},   {-0.5f, 0.5f, -0.5f, 1.0f, 1.0f},
    {-0.5f, -0.5f, -0.5f, 0.0f, 1.0f}, {-0.5f, -0.5f, -0.5f, 0.0f, 1.0f},
    {-0.5f, -0.5f, 0.5f, 0.0f, 0.0f},  {-0.5f, 0.5f, 0.5f, 1.0f, 0.0f},

    {0.5f, 0.5f, 0.5f, 1.0f, 0.0f},    {0.5f, 0.5f, -0.5f, 1.0f, 1.0f},
    {0.5f, -0.5f, -0.5f, 0.0f, 1.0f},  {0.5f, -0.5f, -0.5f, 0.0f, 1.0f},
    {0.5f, -0.5f, 0.5f, 0.0f, 0.0f},   {0.5f, 0.5f, 0.5f, 1.0f, 0.0f},

    {-0.5f, -0.5f, -0.5f, 0.0f, 1.0f}, {0.5f, -0.5f, -0.5f, 1.0f, 1.0f},
    {0.5f, -0.5f, 0.5f, 1.0f, 0.0f},   {0.5f, -0.5f, 0.5f, 1.0f, 0.0f},
    {-0.5f, -0.5f, 0.5f, 0.0f, 0.0f},  {-0.5f, -0.5f, -0.5f, 0.0f, 1.0f},

    {-0.5f, 0.5f, -0.5f, 0.0f, 1.0f},  {0.5f, 0.5f, -0.5f, 1.0f, 1.0f},
    {0.5f, 0.5f, 0.5f, 1.0f, 0.0f},    {0.5f, 0.5f, 0.5f, 1.0f, 0.0f},
    {-0.5f, 0.5f, 0.5f, 0.0f, 0.0f},   {-0.5f, 0.5f, -0.5f, 0.0f, 1.0f}};

GameApplication::GameApplication(int argc, char *argv[])
    : runtime::Application(argc, argv) {}

void GameApplication::onInitialize() {
  runtime::Application::onInitialize();
  _eventbus->on(this, &GameApplication::onMouse);
  _eventbus->on(this, &GameApplication::onKeydown);
  _eventbus->on(this, &GameApplication::onMouseButtonDown);
  _window = new runtime::Window("duskland", 1024, 768);
  _renderer = new video::Renderer();
  _renderer->enableDepthTest();
  shader = new video::Shader(
      {{video::ShaderType::VERTEX, _resources->load("shader::vertex.glsl")},
       {video::ShaderType::FRAGMENT,
        _resources->load("shader::fragment.glsl")}});

  mesh = new video::Mesh({video::POSITION_XYZ, video::TEXCROOD_UV}, vertices);
  auto resource = new runtime::Resource_Buffer("hello world", 11);
  texture = new video::Texture(_resources->load("textures::wall.jpg"));
  texture2 = new video::Texture(_resources->load("textures::container.jpg"));

  projection = glm::perspective(glm::radians(45.0f), (float)1024 / (float)768,
                                0.1f, 100.0f);
  view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
  _renderer->useShader(shader);
  camera = new video::PerspectiveCamera();
  camera->setPosition({0, 0, -3});
  camera->setFront({0, 0, 1});
  keyboard = new input::Keyboard();
  mouse = new input::Mouse();
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
  view = camera->getMatrix();
  _renderer->clear({0.2f, 0.3f, 0.3f, 1.0f});
  model =
      glm::rotate(model, glm::radians(-0.001f), glm::vec3(1.0f, 1.0f, -1.0f));
  _renderer->setTextureUnit("texture0", 0, texture);
  _renderer->setTextureUnit("texture1", 1, texture2);
  shader->setValue("projection", projection);
  shader->setValue("view", view);
  shader->setValue("model", model);
  _renderer->draw(mesh);
  _window->present();
}

void GameApplication::onUnInitialize() { runtime::Application::onInitialize(); }
void GameApplication::onMouse(input::Event_Mouse &e) {
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
void GameApplication::onMouseButtonDown(input::Event_MouseButtonDown &e) {
  if (e.getType() == 1) {
    mouse->catchMouse();
  }
}
void GameApplication::onKeydown(input::Event_KeyDown &e) {
  if (e.getScancode() == SDL_SCANCODE_ESCAPE) {
    mouse->releaseMouse();
  }
  if (e.getScancode() == SDL_SCANCODE_E) {
    if (mouse->isCatched()) {
      mouse->releaseMouse();
    } else {
      mouse->catchMouse();
    }
  }
}