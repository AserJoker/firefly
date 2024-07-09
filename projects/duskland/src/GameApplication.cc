#include "GameApplication.hpp"
#include "core/AutoPtr.hpp"
#include "runtime/Application.hpp"
#include "runtime/Resource_Buffer.hpp"
#include "runtime/Window.hpp"
#include "video/ElementBuffer.hpp"
#include "video/Mesh.hpp"
#include "video/Renderer.hpp"
#include "video/Shader.hpp"
#include "video/Texture.hpp"
#include "video/VertexArray.hpp"
#include <glad/glad.h>
#include <initializer_list>

using namespace firefly;
using namespace duskland;
core::AutoPtr<video::VertexArray> vertexarray;
core::AutoPtr<video::Mesh> mesh;
core::AutoPtr<video::Shader> shader;
core::AutoPtr<video::Texture> texture;
core::AutoPtr<video::Texture> texture2;
struct Vertex {
  float x, y, z, u, v;
};

std::initializer_list<Vertex> vertices = {{-0.5f, -0.5f, 0.f, 0.0f, 0.0f},
                                          {
                                              0.f,
                                              0.5f,
                                              0.f,
                                              0.5f,
                                              1.0f,
                                          },
                                          {0.5f, -0.5f, 0.f, 1.0f, 0.0f}};

std::initializer_list<video::Face> indices = {
    {0, 1, 2}, // first Triangle
};
GameApplication::GameApplication(int argc, char *argv[])
    : runtime::Application(argc, argv) {}  

void GameApplication::onInitialize() {
  runtime::Application::onInitialize();
  _window = new runtime::Window("duskland", 1024, 768);
  _renderer = new video::Renderer();
  shader = new video::Shader(
      {{video::ShaderType::VERTEX, _resources->load("shader::vertex.glsl")},
       {video::ShaderType::FRAGMENT,
        _resources->load("shader::fragment.glsl")}});

  mesh = new video::Mesh({video::POSITION_XYZ, video::TEXCROOD_UV}, vertices,
                         indices);
  auto resource = new runtime::Resource_Buffer("hello world", 11);
  texture = new video::Texture(_resources->load("textures::wall.jpg"));
  texture2 = new video::Texture(_resources->load("textures::container.jpg"));
}

void GameApplication::onMainLoop() {
  runtime::Application::onMainLoop();
  _renderer->clear({0.2f, 0.3f, 0.3f, 1.0f});
  shader->use();
  _renderer->setTextureUnit(shader, "texture0", 0, texture);
  _renderer->setTextureUnit(shader, "texture1", 1, texture2);
  mesh->draw(shader);
  _window->present();
}

void GameApplication::onUnInitialize() { runtime::Application::onInitialize(); }
